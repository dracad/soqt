#ifndef WIDGETRIGGERSEND_H
#define WIDGETRIGGERSEND_H

#include <QWidget>
#include <ports/trigger/generictrigger.h>

namespace Ui {
class WidgeTriggerSend;
}

class WidgeTriggerSend : public GenericTrigger
{
    Q_OBJECT

public:
    explicit WidgeTriggerSend(QWidget *parent = nullptr);
    ~WidgeTriggerSend();

    void populateConfiguration(QXmlStreamWriter &writer);

    void set();

    void loadConfiguration(QXmlStreamReader &reader);
private slots:
    void on_bu_manualSend_clicked();

    void on_bu_delete_clicked();

private:
    Ui::WidgeTriggerSend *ui;

};

#endif // WIDGETRIGGERSEND_H
