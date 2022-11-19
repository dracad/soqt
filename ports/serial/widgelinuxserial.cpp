#include "widgelinuxserial.h"
#include "ui_widgelinuxserial.h"

WidgeLinuxSerial::WidgeLinuxSerial(QWidget *parent) :
    GenericPort(parent),
    ui(new Ui::WidgeLinuxSerial)
{
    ui->setupUi(this);
    ui->out_info->hide();

    auto baudValidator = new QIntValidator(0, 4000000);
    ui->in_baud->setValidator(baudValidator);

    keep_connected_ = false;
    deletion_scheduled_ = false;

    ui->out_title->setText("Serial @" + getUUID() + " - Disconnected");

    scheduled_for_deletion = false;

    //settings
    addSettingHeading("Control Mode Flags");
    registerListSetting("Bits", Serial::allowedBitsPerByte, serSettings_.bitsPerByte, "Number of Bits in a byte");
    registerBoolSetting("Enable Parity", serSettings_.enableParity_, "Enable Parity Bit Check");
    registerBoolSetting("Even Parity", serSettings_.parityEven_, "Check if parity is even, setting ignored if parity is disabled");
    registerBoolSetting("Use 2 Stop bits", serSettings_.use2StopBits_, "If true, 2 stop bits will be used, if false 1");
    registerBoolSetting("Enable Flow Control", serSettings_.flowControl_, "Enable RTS/CTS hardware flow control");
    registerBoolSetting("Enable Hang Up", serSettings_.hangUp_, "Lower modem control lines after last process closes the device (hang up).");
    registerBoolSetting("Enable Reciever", serSettings_.CRead_, "Enables the serial port (probably best not to change this)");
    registerBoolSetting("Disable Modem control lines", serSettings_.CLocal_, "If true, Ignore modem control lines");

    addSettingHeading("Local Mode Flags");
    registerBoolSetting("Enable Cannonical Mode", serSettings_.cannonMode_, "In canonical mode, input is processed when a new line character is received.");
    registerBoolSetting("Enable Signal", serSettings_.ISig_, "When the ISIG bit is set, INTR, QUIT and SUSP characters are interpreted");
    registerBoolSetting("Enable Echo", serSettings_.echo_, "Echo input characters");
    registerBoolSetting("Enable Erasure", serSettings_.erasure_, "If ICANON is also set, the ERASE character erases the "
                                                                     "preceding input character, and WERASE erases the preceding"
                                                                     "word.");
    registerBoolSetting("Enable '\\n' Echo", serSettings_.newLineEcho_, "If ICANON is also set, echo the NL character even if ECHOis not set.");

    addSettingHeading("Input Mode Flags");
    registerBoolSetting("Enable S/W Flow Control", serSettings_.swFlowControl_, "Software flow control");
    registerBoolSetting("Process Data", serSettings_.specialHandle_, "Disable to get raw data, enable will cause linux to process the data");

    addSettingHeading("Output Mode Flags");
    registerBoolSetting("Interpret Chars", serSettings_.outInterpret_, "Special interpretation of output bytes (e.g. newline chars)");
    registerBoolSetting("\\n -> \\n\\r", serSettings_.NLCR_, "conversion of newline to carriage return/line feed");
}

WidgeLinuxSerial::~WidgeLinuxSerial()
{
    if(serial_thread_.isRunning()){
        serial_thread_.quit();
    }

    delete ui;
}

void WidgeLinuxSerial::populateConfiguration(QXmlStreamWriter &writer)
{
    writer.writeTextElement("port", ui->in_port->text());
    writer.writeTextElement("baud", ui->in_baud->text());
}

void WidgeLinuxSerial::loadConfiguration(QXmlStreamReader &reader)
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

void WidgeLinuxSerial::on_bu_delete_clicked()
{
    if(serial_thread_.isRunning()){
        keep_connected_ = false;
        deletion_scheduled_ = true;
        serial_thread_.quit();
    }else{
        this->deleteLater();
    }
}

void WidgeLinuxSerial::ConnectPort()
{

    auto port = ui->in_port->text();
    auto baud = ui->in_baud->text();

    if(baud.toInt() == 0){
        emit consoleLog("Baud Rate is 0. Unable to connect");
        return;
    }

    if(serial_thread_.isRunning()){
        emit consoleLog("Port is still connected, please disconnect all ports before reconnecting");
        return;
    }

    serial_reader *ser = new serial_reader(port.toStdString().c_str(), baud.toInt(), serSettings_);

    ser->moveToThread(&serial_thread_);

    connect(&serial_thread_, &QThread::finished, ser, &QObject::deleteLater);

    connect(ser, &serial_reader::SerialIn, this, [this](const QByteArray &line){
        if(!line.isEmpty()){
            emit Tx(line);
        }

        if(keep_connected_){
            emit operate();
        }
    });
    connect(ser, &serial_reader::connected, this, [this](const QString &port, const int baud){
        emit consoleLog("Successfully opened serial port on port '" + port + "' with baud " + QString::number(baud));
        ui->out_title->setText("Serial @" + getUUID() + " - Connected");
    });
    connect(ser, &serial_reader::failed, this, [this](const QString &port, const int baud){
        emit consoleLog("failed to open serial port on port " + port + " with baud " + QString::number(baud));
    });
    connect(this, &WidgeLinuxSerial::operate, ser, &serial_reader::work);
    connect(this, &WidgeLinuxSerial::write_physical, this, [ser](const QByteArray line){
        ser->write(line);
    });

    connect(ser, &serial_reader::destroyed, this, [this, port](){
        emit consoleLog("Disconnected port");
        ui->out_title->setText("Serial @" + getUUID() + " - Disconnected");
        if(deletion_scheduled_){
            this->deleteLater();
        }
    });

    serial_thread_.start();

    keep_connected_ = true;
    emit operate();
}

void WidgeLinuxSerial::DisconnectPort()
{
    keep_connected_ = false;
    serial_thread_.quit();
}

void WidgeLinuxSerial::dataFromConnectedPort(const QByteArray &data)
{
    emit write_physical(data);
}

__attribute__ ((constructor)) static void RegisterWidgeLinuxSerial(void)
{
  GenericPort::register_constructor("WidgeLinuxSerial", [](){return new WidgeLinuxSerial;});
}

