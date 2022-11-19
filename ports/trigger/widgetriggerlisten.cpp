#include "widgetriggerlisten.h"
#include "ui_widgetriggerlisten.h"

WidgeTriggerListen::WidgeTriggerListen(QWidget *parent) :
    GenericTrigger(parent),
    ui(new Ui::WidgeTriggerListen)
{
    ui->setupUi(this);
    ui->lb_selected->hide();
}

WidgeTriggerListen::~WidgeTriggerListen()
{
    delete ui;
}

void WidgeTriggerListen::set()
{
    ui->lb_selected->show();
}

void WidgeTriggerListen::unset()
{
    ui->lb_selected->hide();
}

void WidgeTriggerListen::populateConfiguration(QXmlStreamWriter &writer)
{
    writer.writeTextElement("data", ui->in_data->text());
    if(ui->in_contains->isChecked()){
        writer.writeTextElement("type", "contains");
    }else{
        writer.writeTextElement("type", "exact");
    }
    if(ui->in_hex->isChecked()){
        writer.writeTextElement("hex", "true");
    }else{
        writer.writeTextElement("hex", "false");
    }
}

void WidgeTriggerListen::loadConfiguration(QXmlStreamReader &reader)
{
    while(reader.readNextStartElement()){
        if(reader.name() == "data"){
            ui->in_data->setText(reader.readElementText());
        }else if(reader.name() == "type"){
            auto type = reader.readElementText();
            if(type == "contains"){
                ui->in_contains->setChecked(true);
            }else{
                ui->in_exact->setChecked(true);
            }
        }else if(reader.name() == "hex"){
            auto type = reader.readElementText();
            if(type == "contains"){
                ui->in_hex->setChecked(true);
            }else{
                ui->in_hex->setChecked(false);
            }
        }else{
            reader.skipCurrentElement();
        }
    }
}

void WidgeTriggerListen::heard(QByteArray data)
{
    QString dt = data;
    if(ui->in_contains->isChecked()){
        if(dt.contains(ui->in_data->text(), Qt::CaseInsensitive)){
            emit goNext();
            ui->lb_selected->hide();
        }
    }

    if(ui->in_exact->isChecked()){
        if(dt.compare(ui->in_data->text()) == 0){
            emit goNext();
            ui->lb_selected->hide();
        }
    }
}

void WidgeTriggerListen::on_bu_delete_clicked()
{
    this->deleteLater();
}

__attribute__ ((constructor)) static void RegisterListen(void)
{
  GenericTrigger::register_constructor("Listener", [](){return new WidgeTriggerListen;});
  GenericTrigger::register_qualified_constructor("WidgeTriggerListen", [](){return new WidgeTriggerListen;});
}

