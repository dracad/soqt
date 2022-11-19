#include "widgetriggerpause.h"
#include "ui_widgetriggerpause.h"

WidgeTriggerPause::WidgeTriggerPause(QWidget *parent) :
    GenericTrigger(parent),
    ui(new Ui::WidgeTriggerPause)
{
    ui->setupUi(this);
    ui->lb_selected->hide();
}

WidgeTriggerPause::~WidgeTriggerPause()
{
    delete ui;
}

void WidgeTriggerPause::set()
{
    ui->lb_selected->show();
    ui->bu_continue->setEnabled(true);
}

void WidgeTriggerPause::unset()
{
    ui->lb_selected->hide();
}

void WidgeTriggerPause::on_bu_delete_clicked()
{
    this->deleteLater();
}


void WidgeTriggerPause::on_bu_continue_clicked()
{
    emit goNext();
}

__attribute__ ((constructor)) static void RegisterSender(void)
{
  GenericTrigger::register_constructor("Pause", [](){return new WidgeTriggerPause;});
  GenericTrigger::register_qualified_constructor("WidgeTriggerPause", [](){return new WidgeTriggerPause;});
}


