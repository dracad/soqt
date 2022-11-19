#include "genericport.h"

GenericPort::GenericPort(QWidget *parent)
    : QWidget{parent}
{
    selected_       = false;
    temp_selected_  = false;

    setObjectName("GenericPort");

    instance_count_++;

    uuid_ = QString::number(instance_count_);
    connected_ = false;

    auto bu = new QPushButton();

    QPixmap pixmap(":/icons/resources/gears-svgrepo-com.svg");
    pixmap = pixmap.scaled(16,16);
    QIcon ButtonIcon(pixmap);
    bu->setIcon(ButtonIcon);
    bu->setIconSize(pixmap.rect().size());

    connect(bu, &QPushButton::clicked, this, &GenericPort::openSetting);

    QTimer::singleShot(10, [this, bu](){
        this->layout()->addWidget(bu);
        this->layout()->setAlignment(bu, Qt::AlignRight);
    });

    settings_.setUUID(getUUID());

    timer_ = new QTimer(this);
    connect(timer_, &QTimer::timeout, this, &GenericPort::processDataBuffer);
}

void GenericPort::createConfiguration(QXmlStreamWriter &writer)
{
    writer.writeStartElement("Settings");

    writer.writeTextElement("enable_delay", QString::number(settings_.get_enable_delay()));
    writer.writeTextElement("enable_split", QString::number(settings_.get_enable_split()));
    auto split = settings_.get_split();
    QString data;

    for(auto &ch:split){
        if(ch == '\n'){
            data += "\\n,";
        }else if(ch == '\r'){
            data += "\\r,";
        }else{
            data += ch + QString(",");
        }
    }
    writer.writeTextElement("split_on", data);
    writer.writeTextElement("delay_value", QString::number(settings_.get_delay()));

    for(auto &setting:settings_list_){
        writer.writeTextElement(setting.name_, setting.getter_());
    }

    writer.writeStartElement("WidgeConfig");
    populateConfiguration(writer);
    writer.writeEndElement();
    writer.writeEndElement();
}

void GenericPort::loadFromConfiguration(QXmlStreamReader &reader)
{
    while(reader.readNextStartElement()){
        if(reader.name() == "Settings"){
            while(reader.readNextStartElement()){
                auto pos = findSetting(reader.name().toString());
                if(reader.name() == "WidgeConfig"){
                    loadConfiguration(reader);
                }else if(reader.name() == "enable_delay"){
                    settings_.enable_delay(reader.readElementText().toInt());
                }else if(reader.name() == "enable_split"){
                    settings_.enable_split(reader.readElementText().toInt());
                }else if(reader.name() == "split_on"){
                    auto charSplit = reader.readElementText().split(",");
                    QList<char> split_on_;

                    for(auto &ch:charSplit){
                        if(ch == "\\n"){
                            split_on_.append('\n');
                        }else if(ch == "\\r"){
                            split_on_.append('\r');
                        }else{
                            if(!ch.isEmpty())
                                split_on_.append(ch.toStdString().at(0));
                        }
                    }

                    settings_.set_split_on(split_on_);
                }else if(reader.name() == "delay_value"){
                    settings_.set_delay(reader.readElementText().toInt());
                }else if(pos > -1){
                    settings_list_.at(pos).setter_(reader.readElementText());
                }else{
                    reader.skipCurrentElement();
                }
            }
        }else{
            reader.skipCurrentElement();
        }
    }
}

void GenericPort::populateConfiguration(QXmlStreamWriter &writer){
    writer.writeTextElement("empty", "empty");
}

void GenericPort::loadConfiguration(QXmlStreamReader &reader){
    while(reader.readNextStartElement()){
        if(reader.name() == "empty"){
            reader.skipCurrentElement();
        }else{
            reader.skipCurrentElement();
        }
    }
}

int GenericPort::findSetting(const QString &name)
{
    for(int x = 0; x < settings_list_.size(); x++){
        if(settings_list_.at(x).name_ == name){
            return x;
        }
    }

    return -1;
}

void GenericPort::Rx(QByteArray data)
{
    auto timeDelay = settings_.get_delay();
    auto timeDelayE = settings_.get_enable_delay();

    auto splitOn = settings_.get_split();
    auto splitOnE = settings_.get_enable_split();

    QList<QByteArray> list;
    list.push_back(data);

    if(splitOnE){
        for(auto &ch:splitOn){
            if(data.contains(ch)){
                list = data.split(ch);
                break;
            }
        }
    }

    dataBuffer_.append(list);

    if(timeDelayE && !timer_->isActive()){
        timer_->start(timeDelay);
    }else if(!timeDelayE && !timer_->isActive()){
        timer_->start(0);
    }
}

void GenericPort::selected(){
    selected_ = true;
    temp_selected_ = false;
    this->repaint();
}

void GenericPort::deselected(){
    selected_ = false;
    this->repaint();
}

void GenericPort::tempSelected()
{
    temp_selected_ = true;
    selected_ = false;
    this->repaint();
}

void GenericPort::tempDeselected()
{
    temp_selected_ = false;
    this->repaint();
}

void GenericPort::RequestConnect()
{
    if(connected_){
        return;
    }

    emit consoleLog("Got Connection request signal");
    this->setEnabled(false);
    connected_ = true;
    ConnectPort();

}

void GenericPort::RequestDisconnect()
{
    if(!connected_){
        return;
    }
    connected_ = false;
    emit consoleLog("Got Disconnect request signal");
    this->setEnabled(true);
    DisconnectPort();

    dataBuffer_.clear();
}

void GenericPort::openSetting()
{
    settings_.exec();
}

void GenericPort::processDataBuffer()
{
    if(!dataBuffer_.isEmpty()){
        auto data = dataBuffer_.front();
        dataBuffer_.pop_front();

        dataFromConnectedPort(data);
        emit dataFromConnection(data);

        if(dataBuffer_.isEmpty()){
            timer_->stop();
        }
    }else{
        timer_->stop();
    }
}

void GenericPort::paintEvent(QPaintEvent *e)
{
    if(selected_){
        QPainter painter(this);
        painter.setPen(QColor(255,0,0));
        painter.drawRoundedRect(0,5,width()-5, height()-7,3,3);
    }else if(temp_selected_){
        QPainter painter(this);
        painter.setPen(QColor(0,255,0));
        painter.drawRoundedRect(0,5,width()-5, height()-7,3,3);
    }else{
        QPainter painter(this);
        painter.setPen(QColor(0,0,0));
        painter.drawRoundedRect(0,5,width()-5, height()-7,3,3);
    }

    QWidget::paintEvent(e);
}

bool GenericPort::fileExists(QString path) {
    QFileInfo check_file(path);
    // check if file exists and if yes: Is it really a file and no directory?
    if (check_file.exists() && check_file.isFile()) {
        return true;
    } else {
        return false;
    }
}

bool GenericPort::directoryExists(QString path) {
    QFileInfo check_file(path);
    // check if directory exists and if yes: Is it really a directory and not a file?
    if (check_file.exists() && check_file.isDir()) {
        return true;
    } else {
        return false;
    }
}

void GenericPort::registerIntSetting(const QString &name, const int minimum, const int maximum, int &value, const QString &details)
{
    settings_.register_int_setting(name, minimum, maximum, value, details);
    QString localName(name);
    settings_list_.push_back({localName.replace(" ", "_").replace("\\", "_").replace("'", "").replace(">", "").replace("<","").replace("/", ""),
                             [&value](){return QString::number(value);},
                             [&value](const QString &newValue){value = newValue.toInt();}});
}

void GenericPort::registerDoubleSetting(const QString &name, const double minimum, const double maximum, double &value, const QString &details)
{
    settings_.register_double_setting(name, minimum, maximum, value, details);

    QString localName(name);
    settings_list_.push_back({localName.replace(" ", "_").replace("\\", "_").replace("'", "").replace(">", "").replace("<","").replace("/", ""),
                             [&value](){return QString::number(value);},
                             [&value](const QString &newValue){value = newValue.toDouble();}});
}

void GenericPort::registerListSetting(const QString &name, const QList<QString> &options, QString &value, const QString &details)
{
    settings_.register_list_setting(name, options, value, details);
    QString localName(name);
    settings_list_.push_back({localName.replace(" ", "_").replace("\\", "_").replace("'", "").replace(">", "").replace("<","").replace("/", ""),
                             [&value](){return value;},
                             [&value](const QString &newValue){value = newValue;}});
}

void GenericPort::registerTextSetting(const QString &name, QString &value, const QString &details)
{
    settings_.register_text_setting(name, value, details);
    QString localName(name);
    settings_list_.push_back({localName.replace(" ", "_").replace("\\", "_").replace("'", "").replace(">", "").replace("<","").replace("/", ""),
                             [&value](){return value;},
                             [&value](const QString &newValue){value = newValue;}});
}

void GenericPort::registerBoolSetting(const QString &name, bool &value, const QString &details)
{
    settings_.register_bool_setting(name, value, details);
    QString localName(name);
    settings_list_.push_back({localName.replace(" ", "_").replace("\\", "_").replace("'", "").replace(">", "").replace("<","").replace("/", ""),
                             [&value](){return QString::number(value);},
                             [&value](const QString &newValue){value = newValue.toInt();}});
}

void GenericPort::addSettingHeading(const QString &name)
{
    settings_.add_heading(name);
}
