#ifndef GREPPER_H
#define GREPPER_H

#include <QWidget>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QLabel>
#include <QRegularExpression>
#include <QDebug>
#include <QGroupBox>

class grepper : public QWidget
{
    Q_OBJECT
public:
    explicit grepper(QWidget *parent = nullptr);

    bool is_valid_text(const QString &text);

signals:
    void changed();

private:
    bool enable;
    bool inverse_match;
    bool caseinsensitive;
    bool match_line;

    QString pattern;
};

#endif // GREPPER_H
