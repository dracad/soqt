#include "widgetriggertag.h"
#include "ui_widgetriggertag.h"

WidgeTriggerTag::WidgeTriggerTag(QWidget *parent) :
    GenericTrigger(parent),
    ui(new Ui::WidgeTriggerTag)
{
    ui->setupUi(this);
    ui->lb_selected->hide();

    ui->in_warning->hide();
}

WidgeTriggerTag::~WidgeTriggerTag()
{
    delete ui;
}

void WidgeTriggerTag::set()
{
    ui->lb_selected->show();
    emit goNext();
    ui->lb_selected->hide();
}

void WidgeTriggerTag::populateConfiguration(QXmlStreamWriter &writer)
{
    writer.writeTextElement("name", ui->in_name->text());
}

void WidgeTriggerTag::loadConfiguration(QXmlStreamReader &reader)
{
    while(reader.readNextStartElement()){
        if(reader.name() == "name"){
            ui->in_name->setText(reader.readElementText());
        }else{
            reader.skipCurrentElement();
        }
    }
}

QString WidgeTriggerTag::tagName()
{
    return ui->in_name->text();
}

void WidgeTriggerTag::nameAlreadyInUse()
{
    ui->in_warning->show();
}

void WidgeTriggerTag::on_in_name_textChanged(const QString &arg1)
{
    ui->in_warning->hide();

    emit tagged(ui->in_name->text());
}


void WidgeTriggerTag::on_bu_delete_clicked()
{
    this->deleteLater();
}

__attribute__ ((constructor)) static void RegisterTag(void)
{
  GenericTrigger::register_constructor("Tag", [](){return new WidgeTriggerTag;});
  GenericTrigger::register_qualified_constructor("WidgeTriggerTag", [](){return new WidgeTriggerTag;});
}

