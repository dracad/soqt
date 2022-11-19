#ifndef GENERICPORT_H
#define GENERICPORT_H

#include <QWidget>
#include <QPainter>
#include <QColor>
#include <QTimer>
#include <QFileInfo>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QMouseEvent>
#include <QPushButton>
#include <QLayout>
#include <QFormLayout>
#include <QQueue>

#include <QDebug>

#include <functional>
#include <random>
#include <sstream>

#include <srcs/portsettingdialog.h>

class GenericPort : public QWidget
{
    Q_OBJECT
    struct Setting
    {
       QString name_;
       std::function<QString (void)> getter_;
       std::function<void(const QString &)> setter_;
    };
public:
    explicit GenericPort(QWidget *parent = nullptr);
    ~GenericPort(){}

    virtual bool supportsRx() = 0;
    virtual bool supportsTx() = 0;

    QString getUUID(){return uuid_;}

    static void register_constructor(const std::string &type, std::function<GenericPort*(void)> construtor){
        if(constructors_.find(type) == constructors_.end()){constructors_.insert({type, construtor});}
    }


    void createConfiguration(QXmlStreamWriter &writer);
    void loadFromConfiguration(QXmlStreamReader &reader);

    static std::map<std::string, std::function<GenericPort*(void)>> getAvailablePortConstructors(){return constructors_;};

signals:
    void Tx(QByteArray data);
    void dataFromConnection(QByteArray data);
    void consoleLog(QString data);

public slots:
    void Rx(QByteArray data);
    void manualSend(QByteArray data){emit Tx(data);}
    void selected();
    void deselected();

    void tempSelected();
    void tempDeselected();

    virtual void on_bu_delete_clicked() = 0;

    void RequestConnect();
    void RequestDisconnect();

private slots:
    void openSetting();
    void processDataBuffer();

protected:
    bool fileExists(QString path);
    bool directoryExists(QString path);

    void registerIntSetting(const QString &name, const int minimum, const int maximum, int &value, const QString &details = "");
    void registerDoubleSetting(const QString &name, const double minimum, const double maximum, double &value, const QString &details = "");
    void registerListSetting(const QString &name, const QList<QString> &options, QString &value, const QString &details = "");
    void registerTextSetting(const QString &name, QString &value, const QString &details = "");
    void registerBoolSetting(const QString &name, bool &value, const QString &details = "");
    void addSettingHeading(const QString &name);

private:
    void paintEvent(QPaintEvent *e);
    virtual void dataFromConnectedPort(const QByteArray &data){};

    virtual void ConnectPort() = 0;
    virtual void DisconnectPort() = 0;
    virtual void populateConfiguration(QXmlStreamWriter &writer);
    virtual void loadConfiguration(QXmlStreamReader &reader);
private:
    bool selected_;
    bool temp_selected_;
    bool connected_;

    QTimer *timer_;

    int findSetting(const QString &name);

    PortSettingDialog settings_;

    QString uuid_;

    QList<Setting> settings_list_;
    QQueue<QByteArray> dataBuffer_;

    inline static int instance_count_ = 0;
    inline static std::map<std::string, std::function<GenericPort*(void)>> constructors_;
};

#endif // GENERICPORT_H
