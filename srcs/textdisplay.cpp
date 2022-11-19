#include "textdisplay.h"

TextDisplay::TextDisplay(QWidget *parent)
    : QWidget{parent}
{
    auto layout = new QVBoxLayout();

    this->setLayout(layout);

    darkGrey = false;

    display     = new QListWidget(this);
    clear       = new QPushButton(this);
    grep_widget = new grepper(this);

    auto viewer_lay = new QHBoxLayout();
    auto manual_send_lay = new QHBoxLayout();
    auto hide_btn = new QPushButton();
    auto show_btn = new QPushButton();
    auto btn_spacer = new QSpacerItem(0,0, QSizePolicy::Expanding, QSizePolicy::Minimum);
    auto text_in = new QLineEdit();
    auto send_btn = new QPushButton();
    auto hex_send_chk = new QCheckBox("As Hex");
    auto hex_view = new QCheckBox("Hex");

    send_btn->setText("->");
    send_btn->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    manual_send_lay->addWidget(text_in);
    manual_send_lay->addWidget(send_btn);
    manual_send_lay->addWidget(hex_send_chk);

    hide_btn->setIcon(QIcon::fromTheme("SP_TitleBarMinButton"));
    show_btn->setIcon(QIcon::fromTheme("SP_TitleBarNormalButton"));

    hide_btn->setText("Hide");
    show_btn->setText("Show");

    viewer_lay->addWidget(hex_view);
    viewer_lay->addSpacerItem(btn_spacer);
    viewer_lay->addWidget(hide_btn);
    viewer_lay->addWidget(show_btn);

    auto copy_selected = new QPushButton(this);

    clear->setText(QStringLiteral("Clear"));
    copy_selected->setText(QStringLiteral("Copy Selected"));

    layout->addWidget(display);
    layout->addWidget(clear);
    layout->addWidget(copy_selected);
    layout->addLayout(manual_send_lay);
    layout->addLayout(viewer_lay);
    layout->addWidget(grep_widget);

    connect(copy_selected, &QPushButton::clicked, this, [this](){
        auto items = display->selectedItems();
        QString text;

        for(auto item:items){
            text += item->text() + "\n";
        }

        QApplication::clipboard()->setText(text);
    });

    connect(clear, &QPushButton::clicked, display, &QListWidget::clear);
    connect(grep_widget, &grepper::changed, this, &TextDisplay::update_lines);

    connect(hide_btn, &QPushButton::clicked, grep_widget, &grepper::hide);
    connect(show_btn, &QPushButton::clicked, grep_widget, &grepper::show);


    auto manual_func = [this, text_in, hex_send_chk](){
        if(text_in->text().isEmpty()){
           return;
        }

        QByteArray data;

        if(hex_send_chk->isChecked()){
            data = QByteArray::fromStdString(Helpers::hexDecode(text_in->text().toStdString()));
        }else{
            data = text_in->text().toUtf8();
        }

        emit send(data);
    };

    connect(send_btn, &QPushButton::clicked, this, manual_func);
    connect(text_in, &QLineEdit::returnPressed, this, manual_func);
    connect(hex_view, &QCheckBox::toggled, this, [this](bool toggled){
        hex = toggled;

        for(int x = 0; x < display->count(); x++){
            auto text = display->item(x);

            if(hex){
                text->setText(Helpers::hexEncode(text->text().toStdString().c_str(), text->text().length(), "").c_str());
            }else{
                text->setText(Helpers::hexDecode(text->text().toStdString()).c_str());
            }
        }

    });
//    connect(this, &TextDisplay::line_added, this, [this](QString &){

//    });

    display->setSelectionMode(QListWidget::ExtendedSelection);
//    display->setStyleSheet("QListWidget::item {border: 1px solid red}");

    hex = false;
}

void TextDisplay::add_line(const QString &line)
{
    std::string ln = line.toStdString();

//    ln.erase(std::remove(ln.begin(), ln.end(), '\n'), ln.end());
//    ln.erase(std::remove(ln.begin(), ln.end(), '\r'), ln.end());
    auto scrollbar = display->verticalScrollBar();

    QListWidgetItem *item;

    if(scrollbar == NULL || scrollbar->value() == scrollbar->maximum()){
        item = new QListWidgetItem(display);
        item->setText(ln.c_str());
        display->scrollToBottom();
    }else{
        item = new QListWidgetItem(display);
        item->setText(ln.c_str());
    }

    if(darkGrey){
        item->setBackground(QColor(180,180,180));
    }else{
        item->setBackground(QColor(200,200,200));
    }

    darkGrey = !darkGrey;

    if(grep_widget->is_valid_text(item->text())){
        item->setHidden(false);
    }else{
        item->setHidden(true);
    }

    emit line_added(item->text());
}

void TextDisplay::update_lines()
{
    for(int x = 0; x < display->count(); x++){
        auto item = display->item(x);

        if(grep_widget->is_valid_text(item->text())){
            if(item->isHidden())
                item->setHidden(false);
        }else{
            if(!item->isHidden())
                item->setHidden(true);
        }
    }
}

void TextDisplay::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Delete){
        auto list = display->selectedItems();

        for(auto &item:list){
            delete item;
        }
    }

    QWidget::keyPressEvent(event);
}
