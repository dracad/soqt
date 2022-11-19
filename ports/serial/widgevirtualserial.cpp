#include "widgevirtualserial.h"
#include "ui_widgevirtualserial.h"

WidgeVirtualSerial::WidgeVirtualSerial(QWidget *parent) :
    GenericPort(parent),
    ui(new Ui::WidgeVirtualSerial)
{
    ui->setupUi(this);

    keep_connected_ = false;
    deletion_scheduled_ = false;

    ui->out_title->setText("Serial @" + getUUID() + " - Disconnected");
}

WidgeVirtualSerial::~WidgeVirtualSerial()
{
    if(!created_link_.isEmpty()){
        std::filesystem::remove(created_link_.toStdString());
    }
    delete ui;
}

void WidgeVirtualSerial::populateConfiguration(QXmlStreamWriter &writer)
{
    writer.writeTextElement("port", ui->in_name->text());
}

void WidgeVirtualSerial::loadConfiguration(QXmlStreamReader &reader)
{
    while(reader.readNextStartElement()){
        if(reader.name() == "port"){
            ui->in_name->setText(reader.readElementText());
        }else{
            reader.skipCurrentElement();
        }
    }
}

void WidgeVirtualSerial::on_bu_delete_clicked()
{
    if(master_thread_.isRunning()){
        keep_connected_ = false;
        deletion_scheduled_ = true;
        master_thread_.quit();
    }else{
        this->deleteLater();
    }
}

void WidgeVirtualSerial::ConnectPort()
{
    if(master_thread_.isRunning()){
        emit consoleLog("Port is still connected, please disconnect all ports before reconnecting");
        return;
    }

    if (openpty(&master_fd_, &slave_fd_, NULL, NULL, NULL) < 0) {
        master_fd_ = -1;
        slave_fd_ = -1;
        emit consoleLog(QString("Error creating virtual port: ") + strerror(errno));
        return;
    }

    ui->lbl_port->setText(ttyname(slave_fd_));
    if(master_fd_ < 0){
        emit consoleLog("Cannot connect to virtual port");
        return;
    }

    std::filesystem::create_symlink(ttyname(slave_fd_), ui->in_name->text().toStdString());
    created_link_ = ui->in_name->text();

    serial_reader *ser = new serial_reader(master_fd_);

    ser->moveToThread(&master_thread_);

    connect(&master_thread_, &QThread::finished, ser, &QObject::deleteLater);

    connect(ser, &serial_reader::SerialIn, this, [this](const QByteArray &line){
        if(!line.isEmpty()){
            emit Tx(line);
        }

        if(keep_connected_){
            emit operate();
        }
    });
    connect(ser, &serial_reader::connected, this, [this](const QString &port, const int baud){
        emit consoleLog("Successfully open serial port on port " + port + "with baud " + QString::number(baud));
    });
    connect(ser, &serial_reader::failed, this, [this](const QString &port, const int baud){
        emit consoleLog("failed to open serial port on port " + port + " with baud " + QString::number(baud));
    });
    connect(this, &WidgeVirtualSerial::operate, ser, &serial_reader::work);
    connect(this, &WidgeVirtualSerial::write_physical, this, [ser](const QByteArray line){
        ser->write(line);
    });

    connect(ser, &serial_reader::destroyed, this, [this](){
        emit consoleLog("Disconnected port");
        ui->out_title->setText("Serial @" + getUUID() + " - Disconnected");
        if(!created_link_.isEmpty()){
            std::filesystem::remove(created_link_.toStdString());
        }
        if(deletion_scheduled_){
            this->deleteLater();
        }
    });

    master_thread_.start();

    keep_connected_ = true;
    emit operate();

    ui->out_title->setText("Serial @" + getUUID() + " - Connected");
}

void WidgeVirtualSerial::DisconnectPort()
{
    keep_connected_ = false;
    master_thread_.quit();
}

void WidgeVirtualSerial::dataFromConnectedPort(const QByteArray &data)
{
    emit write_physical(data);
}

__attribute__ ((constructor)) static void RegisterWidgeVirtualSerial(void)
{
  GenericPort::register_constructor("WidgeVirtualSerial", [](){return new WidgeVirtualSerial;});
}


