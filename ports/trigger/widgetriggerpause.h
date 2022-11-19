#ifndef WIDGETRIGGERPAUSE_H
#define WIDGETRIGGERPAUSE_H

#include <QWidget>

#include <ports/trigger/generictrigger.h>

namespace Ui {
class WidgeTriggerPause;
}

class WidgeTriggerPause : public GenericTrigger
{
    Q_OBJECT

public:
    explicit WidgeTriggerPause(QWidget *parent = nullptr);
    ~WidgeTriggerPause();

    void set();
    void unset();

private slots:
    void on_bu_delete_clicked();

    void on_bu_continue_clicked();

private:
    Ui::WidgeTriggerPause *ui;
};

#endif // WIDGETRIGGERPAUSE_H
