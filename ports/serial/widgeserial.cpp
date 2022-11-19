#include "widgeserial.h"
#include "ui_widgeserial.h"

WidgeSerial::WidgeSerial(QWidget *parent) :
    GenericPort(parent),
    ui(new Ui::WidgeSerial)
{
    ui->setupUi(this);

    auto baudValidator = new QIntValidator(0, 4000000);

    ui->in_baud->setValidator(baudValidator);
    ui->out_info->setHidden(true);

    serial_ = new QSerialPort(this);

    connect(serial_, &QSerialPort::readyRead, this, &WidgeSerial::dataInBuffer);

    ui->out_title->setText("Serial @" + getUUID() + " - Disconnected");
}

WidgeSerial::~WidgeSerial()
{
    delete ui;
}

void WidgeSerial::on_bu_delete_clicked()
{
    this->deleteLater();
}

void WidgeSerial::dataInBuffer()
{
    auto data = serial_->readAll();
    emit Tx(data);
}

void WidgeSerial::ConnectPort()
{
    auto list = QSerialPortInfo::availablePorts();
    for(auto &port:list){
        if(port.portName().compare(ui->in_port->text()) == 0){
            serial_->setPort(port);
            serial_->setBaudRate(ui->in_baud->text().toInt());
            qDebug() << serial_->open(QIODevice::ReadWrite);
            ui->out_title->setText("Serial @" + getUUID() + " - Connected");
        }
    }
}

void WidgeSerial::DisconnectPort()
{
    if(serial_->isOpen())
        serial_->close();

    ui->out_title->setText("Serial @" + getUUID() + " - Disconnected");
}

void WidgeSerial::dataFromConnectedPort(const QByteArray &data)
{
    if(serial_->isOpen()){
        qDebug() << "Writing";
        if(serial_->write(data) < 0){
            emit consoleLog("Error writing to serial port " + serial_->portName() + " error: " + serial_->errorString());
        }
    }
}


void WidgeSerial::on_in_port_textChanged(const QString &arg1)
{
    auto list = QSerialPortInfo::availablePorts();
    for(auto &port:list){
        if(port.portName().compare(arg1) == 0){
            ui->out_info->hide();
            return;
        }
    }

    ui->out_info->show();
    ui->out_info->setToolTip("Port '" + arg1 + "' isnt present on this system");

}

void WidgeSerial::populateConfiguration(QXmlStreamWriter &writer)
{
    writer.writeTextElement("port", ui->in_port->text());
    writer.writeTextElement("baud", ui->in_baud->text());
}

void WidgeSerial::loadConfiguration(QXmlStreamReader &reader)
{
    while(reader.readNextStartElement()){
        if(reader.name() == "port"){
            ui->in_port->setText(reader.readElementText());
        }else if(reader.name() == "baud"){
            ui->in_baud->setText(reader.readElementText());
        }else{
            reader.skipCurrentElement();
        }
    }
}

__attribute__ ((constructor)) static void RegisterWidgeSerial(void)
{
  GenericPort::register_constructor("WidgeSerial", [](){return new WidgeSerial;});
}

