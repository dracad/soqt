#ifndef WIDGELINUXSERIAL_H
#define WIDGELINUXSERIAL_H

#include <QWidget>
#include <QThread>
#include <QIntValidator>

#include <ports/genericport.h>
#include <ports/serial/serial_reader.h>

namespace Ui {
class WidgeLinuxSerial;
}

class WidgeLinuxSerial : public GenericPort
{
    Q_OBJECT

public:
    explicit WidgeLinuxSerial(QWidget *parent = nullptr);
    ~WidgeLinuxSerial();

    bool supportsRx(){return true;}
    bool supportsTx(){return true;}

    void populateConfiguration(QXmlStreamWriter &writer);
    void loadConfiguration(QXmlStreamReader &reader);

private slots:
    void on_bu_delete_clicked();

signals:
    void operate();
    void write_physical(QByteArray data);

private:
    Ui::WidgeLinuxSerial *ui;

    bool keep_connected_;
    bool deletion_scheduled_;

    QThread serial_thread_;

    bool scheduled_for_deletion;

    void ConnectPort();
    void DisconnectPort();

    void dataFromConnectedPort(const QByteArray &data);

private: //settings
    Serial::Settings serSettings_;
};

#endif // WIDGELINUXSERIAL_H
