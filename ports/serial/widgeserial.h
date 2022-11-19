#ifndef WIDGESERIAL_H
#define WIDGESERIAL_H

#include <QWidget>
#include <QMouseEvent>
#include <QDrag>
#include <QIntValidator>
#include <QSerialPort>
#include <QSerialPortInfo>

#include <ports/genericport.h>

#include <QDebug>

namespace Ui {
class WidgeSerial;
}

class WidgeSerial : public GenericPort
{
    Q_OBJECT

public:
    explicit WidgeSerial(QWidget *parent = nullptr);
    ~WidgeSerial();

    bool supportsRx(){return true;}
    bool supportsTx(){return true;}

    void populateConfiguration(QXmlStreamWriter &writer);
    void loadConfiguration(QXmlStreamReader &reader);


private slots:
    void on_bu_delete_clicked();
    void dataInBuffer();

//    void deleteSelf(){this->deleteLater();}

    void on_in_port_textChanged(const QString &arg1);

private:
    Ui::WidgeSerial *ui;

    QSerialPort *serial_;

    void ConnectPort();
    void DisconnectPort();

    void dataFromConnectedPort(const QByteArray &data);

};

#endif // WIDGESERIAL_H
