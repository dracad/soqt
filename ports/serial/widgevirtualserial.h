#ifndef WIDGEVIRTUALSERIAL_H
#define WIDGEVIRTUALSERIAL_H

#include <QWidget>
#include <QThread>

#include <ports/genericport.h>
#include <ports/serial/serial_reader.h>

#include <pty.h>
#include <unistd.h>
#include <filesystem>

namespace Ui {
class WidgeVirtualSerial;
}

class WidgeVirtualSerial : public GenericPort
{
    Q_OBJECT

public:
    explicit WidgeVirtualSerial(QWidget *parent = nullptr);
    ~WidgeVirtualSerial();

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
    Ui::WidgeVirtualSerial *ui;

    QThread master_thread_;

    int master_fd_;
    int slave_fd_;

    bool keep_connected_;
    bool deletion_scheduled_;

    QString created_link_;

    void ConnectPort();
    void DisconnectPort();

    void dataFromConnectedPort(const QByteArray &data);
};

#endif // WIDGEVIRTUALSERIAL_H
