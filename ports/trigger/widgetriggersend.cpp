#include "widgetriggersend.h"
#include "ui_widgetriggersend.h"

WidgeTriggerSend::WidgeTriggerSend(QWidget *parent) :
    GenericTrigger(parent),
    ui(new Ui::WidgeTriggerSend)
{
    ui->setupUi(this);
    ui->lb_selected->hide();
}

WidgeTriggerSend::~WidgeTriggerSend()
{
    delete ui;
}

void WidgeTriggerSend::set()
{
    ui->lb_selected->show();
    QByteArray toSend = ui->in_data->text().replace("\\n", "\n").toUtf8();

    if(ui->in_hex->isChecked()){
        auto str = hexDecode(toSend.toStdString());

        toSend.clear();

        for(auto &ch:str){
            toSend.push_back(ch);
        }
    }

    emit say(toSend);

    ui->lb_selected->hide();

    emit goNext();
}

void WidgeTriggerSend::on_bu_manualSend_clicked()
{
    emit say(ui->in_data->text().replace("\\n", "\n").toUtf8());
}

void WidgeTriggerSend::populateConfiguration(QXmlStreamWriter &writer)
{
    writer.writeTextElement("data", ui->in_data->text());
    if(ui->in_hex->isChecked()){
        writer.writeTextElement("hex", "true");
    }else{
        writer.writeTextElement("hex", "false");
    }
}

void WidgeTriggerSend::loadConfiguration(QXmlStreamReader &reader)
{
    while(reader.readNextStartElement()){
        if(reader.name() == "data"){
            ui->in_data->setText(reader.readElementText());
        }else if(reader.name() == "hex"){
            auto type = reader.readElementText();
            if(type == "true"){
                ui->in_hex->setChecked(true);
            }else{
                ui->in_hex->setChecked(false);
            }
        }else{
            reader.skipCurrentElement();
        }
    }
}


void WidgeTriggerSend::on_bu_delete_clicked()
{
    this->deleteLater();
}

__attribute__ ((constructor)) static void RegisterSender(void)
{
  GenericTrigger::register_constructor("Sender", [](){return new WidgeTriggerSend;});
  GenericTrigger::register_qualified_constructor("WidgeTriggerSend", [](){return new WidgeTriggerSend;});
}

