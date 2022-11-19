#include "widgetriggerwait.h"
#include "ui_widgetriggerwait.h"

WidgeTriggerWait::WidgeTriggerWait(QWidget *parent) :
    GenericTrigger(parent),
    ui(new Ui::WidgeTriggerWait)
{
    ui->setupUi(this);

    auto val = new QIntValidator(0, INT_MAX);
    ui->in_time->setValidator(val);

    ui->lb_selected->hide();
}

WidgeTriggerWait::~WidgeTriggerWait()
{
    delete ui;
}

void WidgeTriggerWait::set()
{
    ui->lb_selected->show();
    size_t waitTime = 0;

    waitTime = ui->in_time->text().toInt();

    if(ui->in_timeSelector->currentText().compare("s") == 0){
        waitTime *= 1000;
    }

    QTimer::singleShot(waitTime, std::bind(&WidgeTriggerWait::emitGoToNext, this));
}

void WidgeTriggerWait::populateConfiguration(QXmlStreamWriter &writer)
{
    writer.writeTextElement("time", ui->in_time->text());
    writer.writeTextElement("selector", ui->in_timeSelector->currentText());
}

void WidgeTriggerWait::loadConfiguration(QXmlStreamReader &reader)
{
    while(reader.readNextStartElement()){
        if(reader.name() == "time"){
            ui->in_time->setText(reader.readElementText());
        }else if(reader.name() == "selector"){
            ui->in_timeSelector->setCurrentText(reader.readElementText());
        }else{
            reader.skipCurrentElement();
        }
    }
}

void WidgeTriggerWait::unset()
{
    ui->lb_selected->hide();
}

void WidgeTriggerWait::emitGoToNext()
{
    ui->lb_selected->hide();
    emit goNext();
}

void WidgeTriggerWait::on_bu_delete_clicked()
{
    this->deleteLater();
}

__attribute__ ((constructor)) static void RegisterWait(void)
{
  GenericTrigger::register_constructor("Wait", [](){return new WidgeTriggerWait;});
  GenericTrigger::register_qualified_constructor("WidgeTriggerWait", [](){return new WidgeTriggerWait;});
}

