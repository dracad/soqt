#ifndef TEXTDISPLAY_H
#define TEXTDISPLAY_H

#include <QWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QPushButton>
#include <QListWidget>
#include <QListWidgetItem>
#include <QClipboard>
#include <QApplication>
#include <QItemSelectionModel>
#include <QKeyEvent>
#include <QIcon>
#include <QSpacerItem>
#include <QSizePolicy>
#include <QHBoxLayout>
#include <QScrollBar>

#include <srcs/grepper.h>

#include <srcs/general_functions.h>

class TextDisplay : public QWidget
{
    Q_OBJECT
public:
    explicit TextDisplay(QWidget *parent = nullptr);

signals:
    void line_added(QString line);
    void send(QByteArray line);

public slots:
    void add_line(const QString &line);
    void update_lines();
    void clearAll(){display->clear();}

    void keyPressEvent(QKeyEvent *event);

private:
    QListWidget     *display;
    QAbstractButton *clear;
    grepper         *grep_widget;

    bool hex;

    bool darkGrey;

};

#endif // TEXTDISPLAY_H
