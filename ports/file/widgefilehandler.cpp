#include "widgefilehandler.h"
#include "ui_widgefilehandler.h"

WidgeFileHandler::WidgeFileHandler(QWidget *parent) :
    GenericPort(parent),
    ui(new Ui::WidgeFileHandler)
{
    ui->setupUi(this);

    on_in_writer_clicked();

    ui->out_title->setText("File @" + getUUID() + " - Disconnected");

    auto rateValidator = new QIntValidator(0, 500);
    ui->in_rate->setValidator(rateValidator);
    ui->lb_info->hide();

    timer_ = new QTimer(this);

    current_line_ = 0;
}

WidgeFileHandler::~WidgeFileHandler()
{
    delete ui;
}

bool WidgeFileHandler::supportsRx()
{
    return ui->in_writer->isChecked();
}

bool WidgeFileHandler::supportsTx()
{
    return ui->in_reader->isChecked();
}

void WidgeFileHandler::populateConfiguration(QXmlStreamWriter &writer)
{
    writer.writeTextElement("file", ui->in_fileLoc->text());
    writer.writeTextElement("rate", ui->in_rate->text());
    if(ui->in_writer->isChecked()){
        writer.writeTextElement("type", "writer");
    }else{
        writer.writeTextElement("type", "reader");
    }
}

void WidgeFileHandler::loadConfiguration(QXmlStreamReader &reader)
{
    while(reader.readNextStartElement()){
        if(reader.name() == "file"){
            ui->in_fileLoc->setText(reader.readElementText());
        }else if(reader.name() == "rate"){
            ui->in_rate->setText(reader.readElementText());
        }else if(reader.name() == "type"){
            auto type = reader.readElementText();
            if(type == "writer"){
                ui->in_writer->setChecked(true);
                on_in_writer_clicked();
            }else{
                ui->in_reader->setChecked(true);
                on_in_reader_clicked();
            }
        }else{
            reader.skipCurrentElement();
        }
    }
}

void WidgeFileHandler::on_bu_delete_clicked()
{
    this->deleteLater();
}

void WidgeFileHandler::ConnectPort()
{
    if(file_.isOpen()){
        emit consoleLog("Please disconnect before reconnecting");
        return;
    }

    file_.setFileName(ui->in_fileLoc->text());

    if(ui->in_writer->isChecked()){
        writer_ = true;
        if (file_.open(QIODevice::WriteOnly)) {
            emit consoleLog("File successfully opened for writing");
        }else{
            emit consoleLog("Unable to open file for writing");
        }
    }else{
        writer_ = false;
        if (file_.open(QIODevice::ReadOnly)) {
            emit consoleLog("File successfully open for reading");
            current_line_ = 0;
            lines_ = file_.readAll().split('\n');

            if(lines_.isEmpty()){
                emit consoleLog("File is empty, nothing to read");
                return;
            }

            if(timer_connection_){
                disconnect(timer_connection_);
            }

            connect(timer_, &QTimer::timeout, this, [this](){
                if(current_line_ < lines_.size()){
                    emit Tx(lines_.at(current_line_) + "\n");
                    current_line_++;
                }else{
                    if(timer_->isActive())
                        timer_->stop();
                }
            });

            timer_->start(1000/ui->in_rate->text().toInt());
        }else{
            emit consoleLog("Unable to open file for reading");
        }
    }
}

void WidgeFileHandler::DisconnectPort()
{
    if(timer_->isActive()){
        timer_->stop();
    }
    if(file_.isOpen()){
        file_.close();
    }
}

void WidgeFileHandler::dataFromConnectedPort(const QByteArray &data)
{
    if(file_.isOpen() && writer_){
        file_.write((data + "\n"));
    }
}


void WidgeFileHandler::on_in_writer_clicked()
{
    ui->lbl_rate->setEnabled(false);
    ui->in_rate->setEnabled(false);

    on_in_fileLoc_textChanged(ui->in_fileLoc->text());
}


void WidgeFileHandler::on_in_reader_clicked()
{
    ui->lbl_rate->setEnabled(true);
    ui->in_rate->setEnabled(true);

    on_in_fileLoc_textChanged(ui->in_fileLoc->text());
}


void WidgeFileHandler::on_in_fileLoc_textChanged(const QString &arg1)
{
    if(ui->in_writer->isChecked()){
        if(fileExists(arg1)){
            ui->lb_info->show();
            ui->lb_info->setToolTip("Warning, file will be overwritten");
        }else{
            ui->lb_info->hide();
        }
    }else{
        if(fileExists(arg1)){
            ui->lb_info->hide();
        }else{
            ui->lb_info->show();
            ui->lb_info->setToolTip("Warning, file does not exist");
        }
    }
}


void WidgeFileHandler::on_bu_openFileLoc_clicked()
{
    QString filename;

    if(ui->in_reader->isChecked()){
        filename = QFileDialog::getOpenFileName(this,
        tr("Open File"), "/home", tr("Files (*)"));
    }else{
        filename = QFileDialog::getSaveFileName(this,
        tr("Save As"), "/home", tr("Files (*)"));
    }

    ui->in_fileLoc->setText(filename);
}

__attribute__ ((constructor)) static void RegisterWidgeFileHandler(void)
{
  GenericPort::register_constructor("WidgeFileHandler", [](){return new WidgeFileHandler;});
}

