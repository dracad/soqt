#include "widgeudp.h"
#include "ui_widgeudp.h"

WidgeUDP::WidgeUDP(QWidget *parent) :
    GenericPort(parent),
    ui(new Ui::WidgeUDP)
{
    ui->setupUi(this);
    on_in_type_selector_currentIndexChanged(1);

    QRegularExpression ipRe("^((25[0-5]|(2[0-4]|1\\d|[1-9]|)\\d)(\\.(?!$)|$)){4}$");

    auto ipValidator = new QRegularExpressionValidator(ipRe, this);
    auto portValidator = new QIntValidator(0,65536,this);

    udpSocket = new QUdpSocket(this);
    remoteSocket = new QUdpSocket(this);

    ui->in_remoteIP->setValidator(ipValidator);
    ui->in_localIP->setValidator(ipValidator);

    ui->in_localPort->setValidator(portValidator);
    ui->in_remotePort->setValidator(portValidator);

    connect(ui->in_remoteIP, &QLineEdit::textChanged, this, &WidgeUDP::checkValidaton);
    connect(ui->in_localIP, &QLineEdit::textChanged, this, &WidgeUDP::checkValidaton);
    connect(ui->in_localPort, &QLineEdit::textChanged, this, &WidgeUDP::checkValidaton);
    connect(ui->in_remotePort, &QLineEdit::textChanged, this, &WidgeUDP::checkValidaton);

    ui->in_remoteIP->setText("0.0.0.0");
    ui->in_localIP->setText("0.0.0.0");
    ui->in_localPort->setText("0");
    ui->in_remotePort->setText("0");

    connect(udpSocket, &QUdpSocket::errorOccurred, this, [this](QAbstractSocket::SocketError err){qDebug() << err;});
    connect(remoteSocket, &QUdpSocket::errorOccurred, this, [this](QAbstractSocket::SocketError err){qDebug() << err;});

    connect(udpSocket, &QUdpSocket::readyRead, this, &WidgeUDP::handleUDPDatagram);

    ui->out_title->setText("UDP @" + getUUID() + " - Disconnected");
}

WidgeUDP::~WidgeUDP()
{
    delete ui;
}

bool WidgeUDP::supportsRx()
{
    return ui->in_remoteIP->isEnabled();
}

bool WidgeUDP::supportsTx()
{
    return ui->in_localIP->isEnabled();
}

void WidgeUDP::populateConfiguration(QXmlStreamWriter &writer)
{
    writer.writeTextElement("remote_ip", ui->in_remoteIP->text());
    writer.writeTextElement("remote_port", ui->in_remotePort->text());
    writer.writeTextElement("local_ip", ui->in_localIP->text());
    writer.writeTextElement("local_port", ui->in_localPort->text());
    writer.writeTextElement("type", ui->in_type_selector->currentText());
}

void WidgeUDP::loadConfiguration(QXmlStreamReader &reader)
{
    while(reader.readNextStartElement()){
        if(reader.name() == "remote_ip"){
            ui->in_remoteIP->setText(reader.readElementText());
        }else if(reader.name() == "remote_port"){
            ui->in_remotePort->setText(reader.readElementText());
        }else if(reader.name() == "local_ip"){
            ui->in_localIP->setText(reader.readElementText());
        }else if(reader.name() == "local_port"){
            ui->in_localPort->setText(reader.readElementText());
        }else if(reader.name() == "type"){
            ui->in_type_selector->setCurrentText(reader.readElementText());
        }else{
            reader.skipCurrentElement();
        }
    }
}

void WidgeUDP::on_in_type_selector_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    auto text = ui->in_type_selector->currentText();

    if(text.compare("Talker") == 0){
        ui->in_localIP->setEnabled(false);
        ui->in_localPort->setEnabled(false);

        ui->in_remotePort->setEnabled(true);
        ui->in_remoteIP->setEnabled(true);

    }else if(text.compare("Listener") == 0){
        ui->in_localIP->setEnabled(true);
        ui->in_localPort->setEnabled(true);

        ui->in_remotePort->setEnabled(false);
        ui->in_remoteIP->setEnabled(false);
    }else{
        ui->in_localIP->setEnabled(true);
        ui->in_localPort->setEnabled(true);
        ui->in_remotePort->setEnabled(true);
        ui->in_remoteIP->setEnabled(true);
    }
}


void WidgeUDP::on_bu_delete_clicked()
{
    this->deleteLater();
}

void WidgeUDP::checkValidaton(QString text)
{
    auto source = dynamic_cast<QLineEdit *>(sender());
    auto pos = text.size();

    if(source->validator()->validate(text, pos) != QValidator::Acceptable){
        source->setStyleSheet("QLineEdit {background: red}");
    }else{
        source->setStyleSheet("QLineEdit {background: rgb(0,255,0)}");
    }

    auto children = this->findChildren<QLineEdit*>();

    for(auto &child:children){
        auto subPos = child->text().size();
        auto subText = child->text();
        if(child->validator()->validate(subText, subPos) != QValidator::Acceptable){
            return;
        }
    }
}

void WidgeUDP::handleUDPDatagram()
{
    while(udpSocket->hasPendingDatagrams()){
        auto datagram = udpSocket->receiveDatagram();
        emit Tx(datagram.data());
    }
}

void WidgeUDP::ConnectPort()
{
    if(udpSocket->state() == QUdpSocket::BoundState){
        udpSocket->disconnectFromHost();
        ui->out_title->setText("UDP @" + getUUID() + " - Disconnected");
    }

    local_port_ = ui->in_localPort->text();
    local_ip_ = ui->in_localIP->text();
    remote_port_ = ui->in_remotePort->text();
    remote_ip_ = ui->in_remoteIP->text();

    auto addr = QHostAddress(ui->in_localIP->text());

    if(ui->in_type_selector->currentText() == "Listener" || ui->in_type_selector->currentText() == "Both"){
        if(!udpSocket->bind(addr, ui->in_localPort->text().toInt())){
            emit consoleLog("[UDP Error]: failed to bind to local ip and port " + ui->in_localIP->text() + ":" + ui->in_localPort->text());
            return;
        }

        if(addr.isMulticast()){
            if(udpSocket->joinMulticastGroup(addr)){
                emit consoleLog("[UDP Success]: Joined multicast group " + ui->in_localIP->text());
            }else{
                emit consoleLog("[UDP Error] Failed to Join Multicast: " + udpSocket->errorString());
                return;
            }
        }

        emit consoleLog("[UDP] Listening to " + local_ip_ + ":" + local_port_);
        ui->out_title->setText("UDP @" + getUUID() + " - Connected");
    }

    if(ui->in_type_selector->currentText() == "Talker" || ui->in_type_selector->currentText() == "Both"){
        remoteSocket->connectToHost(QHostAddress(ui->in_remoteIP->text()), ui->in_remotePort->text().toInt());
        emit consoleLog("[UDP] Sending to " + remote_ip_ + ":" + remote_port_);
        ui->out_title->setText("UDP @" + getUUID() + " - Connected");
    }
}

void WidgeUDP::DisconnectPort()
{
    if(udpSocket->state() == QUdpSocket::ConnectedState){
        udpSocket->disconnectFromHost();
        emit consoleLog("UDP Listener Disconnected. No longer listening to port: " + local_port_);
        ui->out_title->setText("UDP @" + getUUID() + " - Disconnected");
    }

    if(remoteSocket->state() == QUdpSocket::ConnectedState){
        remoteSocket->disconnectFromHost();
        emit consoleLog("UDP Sender Disconnected. No longer sending to " + remote_ip_ + ":" + remote_port_);
        ui->out_title->setText("UDP @" + getUUID() + " - Disconnected");
    }
}

void WidgeUDP::dataFromConnectedPort(const QByteArray &data)
{
    if(remoteSocket->state() == QUdpSocket::ConnectedState){
        QNetworkDatagram datagram;
        datagram.setData(data);
        remoteSocket->writeDatagram(datagram);
    }
}

__attribute__ ((constructor)) static void RegisterWidgeUDP(void)
{
  GenericPort::register_constructor("WidgeUDP", [](){return new WidgeUDP;});
}

