#include "portsettingdialog.h"
#include "ui_portsettingdialog.h"

PortSettingDialog::PortSettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PortSettingDialog)
{
    ui->setupUi(this);
    ui->grp_portSettings->layout()->setContentsMargins(0,0,0,0);

    connect(this, &PortSettingDialog::accepted, this, &PortSettingDialog::process_accepted);

    ui->in_delay->setValidator(new QIntValidator(0, INT_MAX));
    ui->in_splitIn->setValidator(new QRegExpValidator(QRegExp("(\\\\?\\w,)*")));

    enable_split_ = false;
    enable_delay_ = false;

    delay_ = 0;

    currentLoadingLayout_ = nullptr;

    this->setMaximumHeight(QApplication::primaryScreen()->geometry().height() - 100);
    this->setMinimumHeight(QApplication::primaryScreen()->geometry().height() - 100);
}

PortSettingDialog::~PortSettingDialog()
{
    delete ui;
}

void PortSettingDialog::register_int_setting(const QString &name, const int minimum, const int maximum, int &value, const QString &details)
{
    if(currentLoadingLayout_ == nullptr){
        currentLoadingLayout_ = new QFormLayout;
        auto widge = new QWidget();
        widge->setLayout(currentLoadingLayout_);

        ui->grp_portSettings->layout()->addWidget(widge);
    }

    ui->lbl_settings->hide();
    auto holder = new QWidget();
    holder->setLayout(new QHBoxLayout);

    auto in = new QLineEdit();
    auto val = new QIntValidator(minimum, maximum);
    in->setValidator(val);
    holder->layout()->addWidget(in);

    if(!details.isEmpty()){
        QPixmap infoPix(":/icons/resources/info-svgrepo-com.svg");
        infoPix = infoPix.scaled(25, 25);

        auto lbl = new QLabel();
        lbl->setPixmap(infoPix);
        holder->layout()->addWidget(lbl);
        lbl->setToolTip(details);
    }

    connect(this, &PortSettingDialog::accepted, this, [&value, in](){value = in->text().toInt();});
    update_funcs_.push_back([&value, in](){in->setText(QString::number(value));});

    currentLoadingLayout_->addRow(new QLabel(name), holder);
}

void PortSettingDialog::register_double_setting(const QString &name, const double minimum, const double maximum, double &value, const QString &details)
{
    if(currentLoadingLayout_ == nullptr){
        currentLoadingLayout_ = new QFormLayout;
        auto widge = new QWidget();
        widge->setLayout(currentLoadingLayout_);

        ui->grp_portSettings->layout()->addWidget(widge);
    }

    ui->lbl_settings->hide();
    auto holder = new QWidget();
    holder->setLayout(new QHBoxLayout);

    auto in = new QLineEdit();
    auto val = new QDoubleValidator(minimum, maximum, 8);

    in->setValidator(val);
    holder->layout()->addWidget(in);

    if(!details.isEmpty()){
        QPixmap infoPix(":/icons/resources/info-svgrepo-com.svg");
        infoPix = infoPix.scaled(25, 25);

        auto lbl = new QLabel();
        lbl->setPixmap(infoPix);
        holder->layout()->addWidget(lbl);
        lbl->setToolTip(details);
    }

    connect(this, &PortSettingDialog::accepted, this, [&value, in](){value = in->text().toDouble();});
    update_funcs_.push_back([&value, in](){in->setText(QString::number(value));});

    currentLoadingLayout_->addRow(new QLabel(name), holder);
}

void PortSettingDialog::register_list_setting(const QString &name, const QList<QString> &options, QString &value, const QString &details)
{
    if(currentLoadingLayout_ == nullptr){
        currentLoadingLayout_ = new QFormLayout;
        auto widge = new QWidget();
        widge->setLayout(currentLoadingLayout_);

        ui->grp_portSettings->layout()->addWidget(widge);
    }

    ui->lbl_settings->hide();
    auto holder = new QWidget();
    holder->setLayout(new QHBoxLayout);

    auto in = new QComboBox();
    in->addItems(options);

    holder->layout()->addWidget(in);

    if(!details.isEmpty()){
        QPixmap infoPix(":/icons/resources/info-svgrepo-com.svg");
        infoPix = infoPix.scaled(25, 25);

        auto lbl = new QLabel();
        lbl->setPixmap(infoPix);
        holder->layout()->addWidget(lbl);
        lbl->setToolTip(details);
    }

    connect(this, &PortSettingDialog::accepted, this, [&value, in](){value = in->currentText();});
    update_funcs_.push_back([&value, in](){in->setCurrentText(value);});

    currentLoadingLayout_->addRow(new QLabel(name), holder);
}

void PortSettingDialog::register_text_setting(const QString &name, QString &value, const QString &details)
{
    if(currentLoadingLayout_ == nullptr){
        currentLoadingLayout_ = new QFormLayout;
        auto widge = new QWidget();
        widge->setLayout(currentLoadingLayout_);

        ui->grp_portSettings->layout()->addWidget(widge);
    }

    ui->lbl_settings->hide();
    auto holder = new QWidget();
    holder->setLayout(new QHBoxLayout);

    auto in = new QLineEdit();
    holder->layout()->addWidget(in);

    if(!details.isEmpty()){
        QPixmap infoPix(":/icons/resources/info-svgrepo-com.svg");
        infoPix = infoPix.scaled(25, 25);

        auto lbl = new QLabel();
        lbl->setPixmap(infoPix);
        holder->layout()->addWidget(lbl);
        lbl->setToolTip(details);
    }

    connect(this, &PortSettingDialog::accepted, this, [&value, in](){value = in->text();});
    update_funcs_.push_back([&value, in](){in->setText(value);});

    currentLoadingLayout_->addRow(new QLabel(name), holder);
}

void PortSettingDialog::register_bool_setting(const QString &name, bool &value, const QString &details)
{
    if(currentLoadingLayout_ == nullptr){
        currentLoadingLayout_ = new QFormLayout;
        auto widge = new QWidget();
        widge->setLayout(currentLoadingLayout_);

        ui->grp_portSettings->layout()->addWidget(widge);
    }

    ui->lbl_settings->hide();
    auto holder = new QWidget();
    holder->setLayout(new QHBoxLayout);

    auto yes = new QRadioButton("True");
    auto no = new QRadioButton("False");
    holder->layout()->addWidget(yes);
    holder->layout()->addWidget(no);

    if(!details.isEmpty()){
        QPixmap infoPix(":/icons/resources/info-svgrepo-com.svg");
        infoPix = infoPix.scaled(25, 25);

        auto lbl = new QLabel();
        lbl->setPixmap(infoPix);
        holder->layout()->addWidget(lbl);
        lbl->setToolTip(details);
    }

    connect(this, &PortSettingDialog::accepted, this, [&value, yes](){value = yes->isChecked();});
    update_funcs_.push_back([&value, yes, no](){yes->setChecked(value); no->setChecked(!value);});

    currentLoadingLayout_->addRow(new QLabel(name), holder);
}

void PortSettingDialog::add_heading(const QString &headingName)
{
    ui->lbl_settings->hide();
    auto lbl = new QLabel(headingName);
    auto line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    lbl->setMinimumHeight(30);
    lbl->setMaximumHeight(30);

    lbl->setStyleSheet("font-weight: bold;");

    auto holder = new QWidget();
    holder->setLayout(new QVBoxLayout);
    holder->layout()->addWidget(lbl);
    holder->layout()->addWidget(line);
    holder->layout()->setContentsMargins(2, 6, 0, 0);

    holder->layout()->setAlignment(lbl, Qt::AlignHCenter | Qt::AlignBottom);

    currentLoadingLayout_ = new QFormLayout;
    auto widge = new QWidget();
    widge->setLayout(currentLoadingLayout_);

    holder->layout()->addWidget(widge);

    ui->grp_portSettings->layout()->addWidget(holder);
}

void PortSettingDialog::setUUID(const QString &uuid)
{
    ui->out_title->setText("Settings @ " + uuid);
    this->setWindowTitle("Settings");
}

void PortSettingDialog::process_accepted()
{
    enable_split_ = ui->chk_enableSplitIn->isChecked();
    enable_delay_ = ui->chk_delay->isChecked();

    delay_ = ui->in_delay->text().toInt();
    auto charSplit = ui->in_splitIn->text().split(",");
    split_on_.clear();

    for(auto &ch:charSplit){
        if(ch == "\\n"){
            split_on_.append('\n');
        }else if(ch == "\\r"){
            split_on_.append('\r');
        }else{
            if(!ch.isEmpty())
                split_on_.append(ch.toStdString().at(0));
        }
    }
}

void PortSettingDialog::showEvent(QShowEvent *event)
{
    for(auto &update:update_funcs_){
        update();
    }

    ui->chk_enableSplitIn->setChecked(enable_split_);
    ui->chk_delay->setChecked(enable_delay_);

    ui->in_delay->setText(QString::number(delay_));
    QString data;

    for(auto &ch:split_on_){
        if(ch == '\n'){
            data += "\\n,";
        }else if(ch == '\r'){
            data += "\\r,";
        }else{
            data += ch + QString(",");
        }
    }

    ui->in_splitIn->setText(data);

    removeAllMargins(this, 0);
}

void PortSettingDialog::removeAllMargins(QWidget *parent, int depth)
{
  if(parent == nullptr){
    return;
  }

  auto list = parent->findChildren<QWidget *>();
  for(auto &widget:list){
    if(widget == nullptr){
      continue;
    }
    if(widget->layout() != nullptr){
      widget->layout()->setMargin(0);
    }
    removeAllMargins(widget, ++depth);
  }
}
