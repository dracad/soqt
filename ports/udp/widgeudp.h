#ifndef WIDGEUDP_H
#define WIDGEUDP_H

#include <QWidget>
#include <QUdpSocket>
#include <QNetworkDatagram>


#include <ports/genericport.h>

namespace Ui {
class WidgeUDP;
}

class WidgeUDP : public GenericPort
{
    Q_OBJECT

public:
    explicit WidgeUDP(QWidget *parent = nullptr);
    ~WidgeUDP();

    bool supportsRx();
    bool supportsTx();

    void populateConfiguration(QXmlStreamWriter &writer);
    void loadConfiguration(QXmlStreamReader &reader);

private slots:
    void on_in_type_selector_currentIndexChanged(int index);

    void on_bu_delete_clicked();

    void checkValidaton(QString text);
    void handleUDPDatagram();

private:
    Ui::WidgeUDP *ui;

    QUdpSocket *udpSocket;
    QUdpSocket *remoteSocket;

    QString local_port_;
    QString local_ip_;
    QString remote_port_;
    QString remote_ip_;

private:
    void ConnectPort();
    void DisconnectPort();
    void dataFromConnectedPort(const QByteArray &data);
//    void deleteSelf(){this->deleteLater();}
};

#endif // WIDGEUDP_H
