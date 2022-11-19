#ifndef WIDGETRIGGER_H
#define WIDGETRIGGER_H

#include <QWidget>
#include <QInputDialog>
#include <QStringList>

#include <ports/genericport.h>
#include <ports/trigger/generictrigger.h>

#include <ports/trigger/widgetriggergoto.h>
#include <ports/trigger/widgetriggerlisten.h>
#include <ports/trigger/widgetriggersend.h>
#include <ports/trigger/widgetriggertag.h>
#include <ports/trigger/widgetriggerwait.h>

#include <functional>

namespace Ui {
class WidgeTrigger;
}

class WidgeTrigger : public GenericPort
{
    Q_OBJECT

public:
    explicit WidgeTrigger(QWidget *parent = nullptr);
    ~WidgeTrigger();

    bool supportsRx(){return true;}
    bool supportsTx(){return true;}
signals:
    void newTags(std::map<QString, GenericTrigger *>);
private slots:
    void on_bu_delete_clicked();
    void on_bu_addNew_clicked();

    void next();
    void goTo(GenericTrigger *tag);

    void populateConfiguration(QXmlStreamWriter &writer);
    void loadConfiguration(QXmlStreamReader &reader);

private:
    Ui::WidgeTrigger *ui;

    std::map<std::string, std::function<GenericTrigger*(void)>> constructors_;
    QStringList trigger_names_;

    QList<GenericTrigger*> tasks_;
    QList<std::pair<QString, GenericTrigger*>> tags_;

    int current_position_;

    void ConnectPort();
    void DisconnectPort();

    void setupNewTask(GenericTrigger *trigger);
    void dataFromConnectedPort(const QByteArray &data);
    void cleanup();

    bool tagInList(const QString &name);
    int tagPos(const QString &name);
    int tagPos(const GenericTrigger* trig);

    std::map<QString, GenericTrigger *> listToMap(const QList<std::pair<QString, GenericTrigger*>> &list);

    bool running_;
};

#endif // WIDGETRIGGER_H
