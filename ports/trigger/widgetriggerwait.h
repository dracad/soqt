#ifndef WIDGETRIGGERWAIT_H
#define WIDGETRIGGERWAIT_H

#include <QWidget>
#include <QTimer>

#include <ports/trigger/generictrigger.h>

namespace Ui {
class WidgeTriggerWait;
}

class WidgeTriggerWait : public GenericTrigger
{
    Q_OBJECT

public:
    explicit WidgeTriggerWait(QWidget *parent = nullptr);
    ~WidgeTriggerWait();

    void set();
    void unset();

    void populateConfiguration(QXmlStreamWriter &writer);
    void loadConfiguration(QXmlStreamReader &reader);
private slots:
    void on_bu_delete_clicked();

private:
    Ui::WidgeTriggerWait *ui;

    void emitGoToNext();
};

#endif // WIDGETRIGGERWAIT_H
