#ifndef WIDGEFILEHANDLER_H
#define WIDGEFILEHANDLER_H

#include <QWidget>
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <QIntValidator>
#include <QFileDialog>

#include <ports/genericport.h>

namespace Ui {
class WidgeFileHandler;
}

class WidgeFileHandler : public GenericPort
{
    Q_OBJECT

public:
    explicit WidgeFileHandler(QWidget *parent = nullptr);
    ~WidgeFileHandler();

    bool supportsRx();
    bool supportsTx();

    void populateConfiguration(QXmlStreamWriter &writer);
    void loadConfiguration(QXmlStreamReader &reader);

private slots:
    void on_bu_delete_clicked();

    void on_in_writer_clicked();

    void on_in_reader_clicked();

    void on_in_fileLoc_textChanged(const QString &arg1);

    void on_bu_openFileLoc_clicked();

private:
    Ui::WidgeFileHandler *ui;

    QFile file_;

    bool writer_;

    QTimer *timer_;
    QMetaObject::Connection timer_connection_;

    int current_line_;

    QList<QByteArray> lines_;

private:
    void ConnectPort();
    void DisconnectPort();
    void dataFromConnectedPort(const QByteArray &data);
};

#endif // WIDGEFILEHANDLER_H
