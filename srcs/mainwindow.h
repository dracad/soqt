#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QInputDialog>
#include <QGraphicsProxyWidget>
#include <QPlainTextEdit>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QDebug>
#include <QCursor>
#include <QMessageBox>

#include <ports/genericport.h>
#include <ports/serial/widgeserial.h>
#include <ports/serial/widgelinuxserial.h>
#include <ports/serial/widgevirtualserial.h>
#include <ports/udp/widgeudp.h>
#include <ports/gate/widgegate.h>
#include <ports/file/widgefilehandler.h>
#include <ports/trigger/widgetrigger.h>
#include <ports/tcp/widgetcp.h>


#include <srcs/myscene.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_bu_addSerial_clicked();

    void on_bu_addUDP_clicked();

    void on_bu_addGate_clicked();

    void on_bu_addFile_clicked();

    void setSelectedPort(GenericPort *port);
    void deselectPort();

    void consoleLog(const QString &msg);

    void on_bu_addLinuxSerial_clicked();

    void on_bu_addVirtSerial_clicked();

    void on_bu_addTriggers_clicked();

    void on_actionSave_As_triggered();

    void on_actionSave_triggered();

    void on_actionOpen_triggered();

    void on_bu_connectionTool_clicked();

    void on_bu_selectTool_clicked();

    void on_bu_moveTool_clicked();

    void on_actionClear_All_triggered();

    void on_actionNew_triggered();

    void on_bu_addTCP_clicked();

    void on_actionToggle_Monitor_triggered();

    void on_actionToggle_Designer_triggered();

    void on_actionToggle_Console_triggered();

private:
    Ui::MainWindow *ui;

    QList<QString> accepted_types_;
    QList<GenericPort *> ports_;

    GenericPort *monitored_port_;
    QMetaObject::Connection connection_physical_monitor_;
    QMetaObject::Connection connection_physical_manual_send_;

    QMetaObject::Connection connection_connected_monitor_;
    QMetaObject::Connection connection_connect_manual_send_;

    bool eventFilter(QObject *watched, QEvent *event);
    void clearAll();

private:
    void setupNewPort(GenericPort *port, const QString &type);

    int findPort(GenericPort *port);

    std::map<QString, GenericPort*> addPortsFromFile(QXmlStreamReader &reader);
    void addConnectionsFromfile(QXmlStreamReader &reader, std::map<QString, GenericPort *> &ids);

    QString currentFileName_;

    WorkingTool current_selected_tool_;
    std::map<WorkingTool, QString> tool_to_icon_;
};
#endif // MAINWINDOW_H
