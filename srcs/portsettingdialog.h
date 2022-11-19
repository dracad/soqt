#ifndef PORTSETTINGDIALOG_H
#define PORTSETTINGDIALOG_H

#include <QDialog>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QIntValidator>
#include <QDoubleValidator>
#include <QRegExpValidator>
#include <QIcon>
#include <QDebug>
#include <QComboBox>
#include <QRadioButton>
#include <QString>
#include <QFormLayout>

#include <QScreen>

#include <functional>

namespace Ui {
class PortSettingDialog;
}

class PortSettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PortSettingDialog(QWidget *parent = nullptr);
    ~PortSettingDialog();

    void register_int_setting(const QString &name, const int minimum, const int maximum, int &value, const QString &details = "");
    void register_double_setting(const QString &name, const double minimum, const double maximum, double &value, const QString &details = "");
    void register_list_setting(const QString &name, const QList<QString> &options, QString &value, const QString &details = "");
    void register_text_setting(const QString &name, QString &value, const QString &details = "");
    void register_bool_setting(const QString &name, bool &value, const QString &details = "");
    void add_heading(const QString &headingName);

    void setUUID(const QString &uuid);

    void enable_split(bool split){enable_split_ = split;}
    void enable_delay(bool delay){enable_delay_ = delay;}

    void set_split_on(const QList<char> &split){split_on_ = split;}
    void set_delay(int delay){delay_ = delay;}

    bool get_enable_split(){return enable_split_;}
    bool get_enable_delay(){return enable_delay_;}

    QList<char> get_split(){return split_on_;}
    int get_delay(){return delay_;}

private slots:
    void process_accepted();
private:
    Ui::PortSettingDialog *ui;

    QFormLayout *currentLoadingLayout_;

    void showEvent( QShowEvent* event );

    QList<std::function<void(void)>> update_funcs_;

    bool enable_delay_;
    bool enable_split_;

    QList<char> split_on_;
    int delay_;



    void removeAllMargins(QWidget *parent, int depth);
};

#endif // PORTSETTINGDIALOG_H
