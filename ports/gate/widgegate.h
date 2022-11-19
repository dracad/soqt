#ifndef WIDGEGATE_H
#define WIDGEGATE_H

#include <QWidget>
#include <ports/genericport.h>

namespace Ui {
class WidgeGate;
}

class WidgeGate : public GenericPort
{
    Q_OBJECT

public:
    explicit WidgeGate(QWidget *parent = nullptr);
    ~WidgeGate();

    bool supportsRx(){return true;}
    bool supportsTx(){return true;}

private slots:
    void on_bu_delete_clicked();

private:
    Ui::WidgeGate *ui;

    void ConnectPort(){};
    void DisconnectPort(){};

    void dataFromConnectedPort(const QByteArray &data);
};

#endif // WIDGEGATE_H
