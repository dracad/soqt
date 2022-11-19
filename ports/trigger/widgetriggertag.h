#ifndef WIDGETRIGGERTAG_H
#define WIDGETRIGGERTAG_H

#include <QWidget>
#include <ports/trigger/generictrigger.h>

namespace Ui {
class WidgeTriggerTag;
}

class WidgeTriggerTag : public GenericTrigger
{
    Q_OBJECT

public:
    explicit WidgeTriggerTag(QWidget *parent = nullptr);
    ~WidgeTriggerTag();

    void set();

    void nameAlreadyInUse();

    void populateConfiguration(QXmlStreamWriter &writer);
    void loadConfiguration(QXmlStreamReader &reader);

    QString tagName();
private slots:
    void on_in_name_textChanged(const QString &arg1);

    void on_bu_delete_clicked();

private:
    Ui::WidgeTriggerTag *ui;
};


#endif // WIDGETRIGGERTAG_H
