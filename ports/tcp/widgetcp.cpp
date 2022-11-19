#include "widgetcp.h"
#include "ui_widgetcp.h"

WidgeTCP::WidgeTCP(QWidget *parent) :
    GenericPort(parent),
    ui(new Ui::WidgeTCP)
{
    ui->setupUi(this);

    ui->out_title->setText("TCP @" + getUUID() + " - Disconnected");

    QRegularExpression ipRe("^((25[0-5]|(2[0-4]|1\\d|[1-9]|)\\d)(\\.(?!$)|$)){4}$");

    auto ipValidator = new QRegularExpressionValidator(ipRe, this);
    auto portValidator = new QIntValidator(0,65536,this);

    ui->in_port->setValidator(portValidator);
    ui->in_ip->setValidator(ipValidator);


    connect(ui->in_port, &QLineEdit::textChanged, this, &WidgeTCP::checkValidaton);
    connect(ui->in_ip, &QLineEdit::textChanged, this, &WidgeTCP::checkValidaton);

    ui->in_ip->setText("0.0.0.0");
    ui->in_port->setText("0");

    server_ = new QTcpServer(this);

    client_sock_ = new QTcpSocket(this);

    connect(client_sock_, &QTcpSocket::readyRead, this, &WidgeTCP::handleDataIn);
    connect(client_sock_, &QTcpSocket::errorOccurred, this, &WidgeTCP::socketError);
    connect(client_sock_, &QTcpSocket::connected, this, [this](){emit consoleLog("Client Connected to host");});
    connect(client_sock_, &QTcpSocket::disconnected, this, [this](){emit consoleLog("Client disconnected from host");});
}

WidgeTCP::~WidgeTCP()
{
    delete ui;
}

void WidgeTCP::checkValidaton(QString text)
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

void WidgeTCP::populateConfiguration(QXmlStreamWriter &writer)
{
    writer.writeTextElement("ip", ui->in_ip->text());
    writer.writeTextElement("port", ui->in_port->text());
    writer.writeTextElement("type", ui->in_type->currentText());
}

void WidgeTCP::loadConfiguration(QXmlStreamReader &reader)
{
    while(reader.readNextStartElement()){
        if(reader.name() == "ip"){
            ui->in_ip->setText(reader.readElementText());
        }else if(reader.name() == "port"){
            ui->in_port->setText(reader.readElementText());
        }else if(reader.name() == "type"){
            ui->in_type->setCurrentText(reader.readElementText());
        }else{
            reader.skipCurrentElement();
        }
    }
}

void WidgeTCP::on_bu_delete_clicked()
{
    this->deleteLater();
}

void WidgeTCP::acceptNewConnection()
{
    auto sock = server_->nextPendingConnection();
    sockets_.push_back(sock);

    emit consoleLog("Accepted new connection");

    connect(sock, &QTcpSocket::readyRead, this, &WidgeTCP::handleDataIn);
    connect(sock, &QTcpSocket::disconnected, this, &WidgeTCP::deleteSocket);
    connect(sock, &QTcpSocket::errorOccurred, this, &WidgeTCP::socketError);
}

void WidgeTCP::handleDataIn()
{
    QTcpSocket* socket = reinterpret_cast<QTcpSocket*>(sender());

    while (socket->bytesAvailable())
    {
        QByteArray temp = socket->readAll();
        emit Tx(temp);
    }
}

void WidgeTCP::deleteSocket()
{
    QTcpSocket* socket = reinterpret_cast<QTcpSocket*>(sender());
    sockets_.removeOne(socket);
    socket->deleteLater();
    emit consoleLog("Closed connection to peer");
}

void WidgeTCP::socketError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
        case QAbstractSocket::RemoteHostClosedError:
        break;
        case QAbstractSocket::HostNotFoundError:
            emit consoleLog("TCP, The host was not found. Please check the host name and port settings.");
        break;
        case QAbstractSocket::ConnectionRefusedError:
            emit consoleLog("TCP, The connection was refused by the peer. Make sure QTCPServer is running, and check that the host name and port settings are correct.");
        break;
        default:
            QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
            emit consoleLog("TCP," + QString("The following error occurred: %1.").arg(socket->errorString()));
        break;
    }
}

void WidgeTCP::ConnectPort()
{
    if(ui->in_type->currentText() == "Server"){
        connect(server_, &QTcpServer::newConnection, this, &WidgeTCP::acceptNewConnection, Qt::UniqueConnection);

        if(!server_->listen(QHostAddress(ui->in_ip->text()), ui->in_port->text().toInt())){
            emit consoleLog("Error on server listen: " + server_->errorString());
        }
    }else{
        client_sock_->connectToHost(QHostAddress(ui->in_ip->text()), ui->in_port->text().toInt());
    }
}

void WidgeTCP::DisconnectPort()
{
    for(auto &sock:sockets_){
        sock->close();
    }

    if(server_->isListening()){
        server_->close();
    }

    if(client_sock_->isOpen()){
        client_sock_->close();
    }
}

void WidgeTCP::dataFromConnectedPort(const QByteArray &data)
{
    for(auto &sock:sockets_){
        sock->write(data);
    }

    if(client_sock_->state() == QAbstractSocket::ConnectedState){
       client_sock_->close();
    }
}

__attribute__ ((constructor)) static void RegisterWidgeTCP(void)
{
  GenericPort::register_constructor("WidgeTCP", [](){return new WidgeTCP;});
}


