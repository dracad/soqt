#ifndef WIDGETRIGGERLISTEN_H
#define WIDGETRIGGERLISTEN_H

#include <QWidget>
#include <ports/trigger/generictrigger.h>

namespace Ui {
class WidgeTriggerListen;
}

class WidgeTriggerListen : public GenericTrigger
{
    Q_OBJECT

public:
    explicit WidgeTriggerListen(QWidget *parent = nullptr);
    ~WidgeTriggerListen();

    void set();
    void unset();

    void populateConfiguration(QXmlStreamWriter &writer);
    void loadConfiguration(QXmlStreamReader &reader);
private:
    Ui::WidgeTriggerListen *ui;
public slots:
    void heard(QByteArray data);
private slots:
    void on_bu_delete_clicked();
};


#endif // WIDGETRIGGERLISTEN_H
