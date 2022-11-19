#include "widgegate.h"
#include "ui_widgegate.h"

WidgeGate::WidgeGate(QWidget *parent) :
    GenericPort(parent),
    ui(new Ui::WidgeGate)
{
    ui->setupUi(this);
    ui->bu_gate->setEnabled(false);

    ui->out_title->setText("Gate @" + getUUID());
}

WidgeGate::~WidgeGate()
{
    delete ui;
}

void WidgeGate::on_bu_delete_clicked()
{
    this->deleteLater();
}

void WidgeGate::dataFromConnectedPort(const QByteArray &data)
{
    emit Tx(data);
}

__attribute__ ((constructor)) static void RegisterWidgeGate(void)
{
  GenericPort::register_constructor("WidgeGate", [](){return new WidgeGate;});
}

