#ifndef WIDGETRIGGERGOTO_H
#define WIDGETRIGGERGOTO_H

#include <QWidget>
#include <ports/trigger/generictrigger.h>

namespace Ui {
class WidgeTriggerGoTo;
}

class WidgeTriggerGoTo : public GenericTrigger
{
    Q_OBJECT

public:
    explicit WidgeTriggerGoTo(QWidget *parent = nullptr);
    ~WidgeTriggerGoTo();

    void setAvailableTriggers(std::map<QString, GenericTrigger*> triggers);

    void populateConfiguration(QXmlStreamWriter &writer);
    void loadConfiguration(QXmlStreamReader &reader);
    void set();

private slots:
    void on_bu_delete_clicked();

    void availableTags(std::map<QString, GenericTrigger*> tags);

private:
    Ui::WidgeTriggerGoTo *ui;

    std::map<QString, GenericTrigger*> available_triggers_;


};

#endif // WIDGETRIGGERGOTO_H
