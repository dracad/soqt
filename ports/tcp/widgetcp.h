#ifndef WIDGETCP_H
#define WIDGETCP_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>

#include <ports/genericport.h>

namespace Ui {
class WidgeTCP;
}

class WidgeTCP : public GenericPort
{
    Q_OBJECT

public:
    explicit WidgeTCP(QWidget *parent = nullptr);
    ~WidgeTCP();

    bool supportsRx(){return true;}
    bool supportsTx(){return true;}

    void populateConfiguration(QXmlStreamWriter &writer);
    void loadConfiguration(QXmlStreamReader &reader);

private slots:
    void on_bu_delete_clicked();
    void acceptNewConnection();
    void handleDataIn();
    void deleteSocket();
    void socketError(QAbstractSocket::SocketError socketError);

private:
    Ui::WidgeTCP *ui;
    QTcpServer *server_;
    QTcpSocket *client_sock_;

    QList<QTcpSocket*> sockets_;

    void ConnectPort();
    void DisconnectPort();
    void dataFromConnectedPort(const QByteArray &data);
    void checkValidaton(QString text);


};

#endif // WIDGETCP_H
