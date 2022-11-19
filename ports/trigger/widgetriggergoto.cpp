#include "widgetriggergoto.h"
#include "ui_widgetriggergoto.h"

WidgeTriggerGoTo::WidgeTriggerGoTo(QWidget *parent) :
    GenericTrigger(parent),
    ui(new Ui::WidgeTriggerGoTo)
{
    ui->setupUi(this);
    ui->lb_selected->hide();
}

WidgeTriggerGoTo::~WidgeTriggerGoTo()
{
    delete ui;
}

void WidgeTriggerGoTo::setAvailableTriggers(std::map<QString, GenericTrigger *> triggers)
{
    available_triggers_ = triggers;

    ui->in_tag->clear();

    for(auto &[name, trig]:available_triggers_){

        ui->in_tag->addItem(name);
    }
}

void WidgeTriggerGoTo::populateConfiguration(QXmlStreamWriter &writer)
{
    writer.writeTextElement("goto", ui->in_tag->currentText());
}

void WidgeTriggerGoTo::loadConfiguration(QXmlStreamReader &reader)
{
    while(reader.readNextStartElement()){
        if(reader.name() == "goto"){
            ui->in_tag->setCurrentText(reader.readElementText());
        }else{
            reader.skipCurrentElement();
        }
    }
}

void WidgeTriggerGoTo::set()
{
    ui->lb_selected->show();
    emit goTo(available_triggers_[ui->in_tag->currentText()]);
    ui->lb_selected->hide();
}

void WidgeTriggerGoTo::on_bu_delete_clicked()
{
    this->deleteLater();
}

void WidgeTriggerGoTo::availableTags(std::map<QString, GenericTrigger *> tags)
{
    available_triggers_ = tags;

    ui->in_tag->clear();

    for(auto &tag:available_triggers_){
        ui->in_tag->addItem(tag.first);
    }
}

__attribute__ ((constructor)) static void RegisterGoTo(void)
{
  GenericTrigger::register_constructor("GoTo", [](){return new WidgeTriggerGoTo;});
  GenericTrigger::register_qualified_constructor("WidgeTriggerGoTo", [](){return new WidgeTriggerGoTo;});
}
