#include "widgetrigger.h"
#include "ui_widgetrigger.h"

WidgeTrigger::WidgeTrigger(QWidget *parent) :
    GenericPort(parent),
    ui(new Ui::WidgeTrigger)
{
    ui->setupUi(this);

//    GenericTrigger::register_constructor("GoTo", [](){return new WidgeTriggerGoTo;});
//    GenericTrigger::register_constructor("Wait", [](){return new WidgeTriggerWait;});
//    GenericTrigger::register_constructor("Send", [](){return new WidgeTriggerSend;});
//    GenericTrigger::register_constructor("Tag", [](){return new WidgeTriggerTag;});
//    GenericTrigger::register_constructor("Listen", [](){return new WidgeTriggerListen;});

    constructors_ = GenericTrigger::getAvailableTriggers();

    for(auto &[name, constructor]:constructors_){
        trigger_names_.push_back(name.c_str());
    }

    current_position_ = -1;

    ui->out_title->setText("Tasks @" + getUUID());
    running_ = false;
}

WidgeTrigger::~WidgeTrigger()
{
    cleanup();
    for(auto &task:tasks_){
        delete task;
    }

    delete ui;
}

void WidgeTrigger::ConnectPort()
{
    current_position_ = -1;
    running_ = true;
    next();
}

void WidgeTrigger::DisconnectPort()
{
    cleanup();
}

void WidgeTrigger::setupNewTask(GenericTrigger *trigger)
{
    ui->tasks_container->layout()->addWidget(trigger);

    tasks_.push_back(trigger);

    connect(trigger, &QWidget::destroyed, this, [trigger, this](){
        tasks_.removeOne(trigger);
        auto pos = tagPos(trigger);
        if(pos >= 0){
            tags_.erase(tags_.begin() + pos);
            emit newTags(listToMap(tags_));
        }
    });
    connect(trigger, &GenericTrigger::goNext, this, &WidgeTrigger::next);
    connect(trigger, &GenericTrigger::say, this, [this](QByteArray msg){emit Tx(msg);});
    connect(trigger, &GenericTrigger::goTo, this, &WidgeTrigger::goTo);
    connect(trigger, &GenericTrigger::tagged, this, [this, trigger](QString newTag){
        if(!tagInList(newTag)){
            auto pos = tagPos(trigger);
            if(pos < 0){
                tags_.push_back({newTag, trigger});
            }else{
                qDebug() << "here1";
                tags_[pos].first = newTag;
            }
        }else{
            auto trigpos = tagPos(trigger);
            auto tagpos = tagPos(newTag);

            if(trigpos != tagpos){
                trigger->nameAlreadyInUse();
            }
        }
        emit newTags(listToMap(tags_));
    });

    connect(this, &WidgeTrigger::newTags, trigger, &GenericTrigger::availableTags);
    trigger->availableTags(listToMap(tags_));
}

void WidgeTrigger::dataFromConnectedPort(const QByteArray &data)
{
    if(current_position_ >= tasks_.size() || current_position_ < 0){
        return;
    }

    tasks_.at(current_position_)->heard(data);
}

void WidgeTrigger::cleanup()
{
    running_ = false;
    if(current_position_ >= tasks_.size() || current_position_ < 0){
        return;
    }

    tasks_.at(current_position_)->unset();
    current_position_ = -1;
}

bool WidgeTrigger::tagInList(const QString &name)
{
    for(auto &[tag, task]:tags_){
        if(name.compare(tag) == 0){
            return true;
        }
    }

    return false;
}

int WidgeTrigger::tagPos(const QString &name)
{
    for(int x = 0; x < tags_.size(); x++){
        if(tags_.at(x).first.compare(name) == 0 ){
            return x;
        }
    }

    return -1;
}

int WidgeTrigger::tagPos(const GenericTrigger *trig)
{
    for(int x = 0; x < tags_.size(); x++){
        if(tags_.at(x).second == trig ){
            return x;
        }
    }

    return -1;
}

std::map<QString, GenericTrigger *> WidgeTrigger::listToMap(const QList<std::pair<QString, GenericTrigger *> > &list)
{
    std::map<QString, GenericTrigger *> mapping;

    for(auto &[tag, task]:list){
        mapping.insert({tag, task});
    }

    return mapping;
}

void WidgeTrigger::on_bu_delete_clicked()
{
    this->deleteLater();
}

void WidgeTrigger::on_bu_addNew_clicked()
{
    bool ok;
    QString text = QInputDialog::getItem(nullptr, tr("Add new Task"), tr("Select Task type"), trigger_names_, 0, false, &ok);

    if (ok && !text.isEmpty()){
        auto trigger = constructors_[text.toStdString()]();
        setupNewTask(trigger);
    }
}

void WidgeTrigger::next()
{
    if(!running_){
        return;
    }

    current_position_++;

    if(current_position_ >= tasks_.size() || current_position_ < 0){
        emit consoleLog("All tasks completed");
        return;
    }

    tasks_.at(current_position_)->set();
}

void WidgeTrigger::goTo(GenericTrigger *tag)
{
    for(int x = 0; x < tasks_.size(); x++){
        if(tasks_.at(x) == tag){
            current_position_ = x - 1;
            next();
        }
    }
}

void WidgeTrigger::populateConfiguration(QXmlStreamWriter &writer)
{
    writer.writeStartElement("tasks");
    for(auto &task:tasks_){
        writer.writeStartElement("task");
        writer.writeTextElement("type", task->metaObject()->className());
        writer.writeStartElement("configuration");
        task->populateConfiguration(writer);
        writer.writeEndElement();
        writer.writeEndElement();
    }
    writer.writeEndElement();
}

void WidgeTrigger::loadConfiguration(QXmlStreamReader &reader)
{
    auto qualified_cons = GenericTrigger::getQualifiedConstructors();

    while(reader.readNextStartElement()){
        if(reader.name() == "tasks"){
            while(reader.readNextStartElement()){
                if(reader.name() == "task"){
                    GenericTrigger *task = nullptr;
                    while(reader.readNextStartElement()){
                        if(reader.name() == "type"){
                            auto type = reader.readElementText();
                            if(qualified_cons.find(type.toStdString()) != qualified_cons.end()){
                                task = qualified_cons[type.toStdString()]();
                                setupNewTask(task);
//                                QTimer::singleShot(10, [this, task](){setupNewTask(task);});
                            }
                        }else if(reader.name() == "configuration"){
                            if(task != nullptr){

                                task->loadConfiguration(reader);
                            }else{
                                qDebug() << "here";
                            }
                        }
                    }
                }else{
                    reader.skipCurrentElement();
                }
            }
        }else{
            reader.skipCurrentElement();
        }
    }
}



__attribute__ ((constructor)) static void RegisterTrigger(void)
{
  GenericPort::register_constructor("WidgeTrigger", [](){return new WidgeTrigger;});
}

