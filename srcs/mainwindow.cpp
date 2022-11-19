#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    monitored_port_ = nullptr;

    ui->lst_ports->setItemText(ui->lst_ports->indexOf(ui->pg_ports), QStringLiteral("Ports"));
    ui->port_monitor_container->hide();

    accepted_types_ = {QStringLiteral("Serial"), QStringLiteral("UDP")};

    auto scene = new MyScene();

    scene->installEventFilter(this);

    ui->out_mainview->setScene(scene);
    ui->out_mainview->scene()->setSceneRect(0,0,1920,1080);

    connect(scene, &MyScene::portSelected, this, &MainWindow::setSelectedPort);
    connect(scene, &MyScene::portDeselected, this, &MainWindow::deselectPort);

    connect(scene, &MyScene::consoleLog, this, &MainWindow::consoleLog);

//    connect(ui->bu_fitInView, &QPushButton::clicked, ui->out_mainview,  &PortView::fitSceneInView);

    this->setWindowTitle("soqt @ None");

    current_selected_tool_ = WorkingTool::Cursor;

    tool_to_icon_.insert({WorkingTool::Connecting, ":/icons/resources/flow-connection-svgrepo-com.svg"});
    tool_to_icon_.insert({WorkingTool::Dragging, ":/icons/resources/move-svgrepo-com.svg"});

    ui->active_block_lay->setAlignment(ui->ports_top_lay, Qt::AlignLeft);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_bu_addSerial_clicked()
{
    auto port = new WidgeSerial();
    setupNewPort(port, QStringLiteral("Serial"));
}


void MainWindow::on_bu_addUDP_clicked()
{
    auto port = new WidgeUDP();
    setupNewPort(port, QStringLiteral("UDP"));
}


void MainWindow::on_bu_addGate_clicked()
{
    auto port = new WidgeGate();
    setupNewPort(port, QStringLiteral("Gate"));
}


void MainWindow::on_bu_addFile_clicked()
{
    auto port = new WidgeFileHandler();
    setupNewPort(port, QStringLiteral("File"));
}


void MainWindow::on_bu_addLinuxSerial_clicked()
{
    auto port = new WidgeLinuxSerial();
    setupNewPort(port, QStringLiteral("Linux Serial"));
}

void MainWindow::on_bu_addVirtSerial_clicked()
{
    auto port = new WidgeVirtualSerial();
    setupNewPort(port, QStringLiteral("Virtual Serial"));
}


void MainWindow::on_bu_addTriggers_clicked()
{
    auto port = new WidgeTrigger();
    setupNewPort(port, QStringLiteral("Virtual Serial"));
}

void MainWindow::on_bu_addTCP_clicked()
{
    auto port = new WidgeTCP();
    setupNewPort(port, QStringLiteral("TCP"));
}


void MainWindow::setSelectedPort(GenericPort *port)
{
//    if(port != monitored_port_){
//        qDebug() << port << " | " << monitored_port_;
//        ui->text_in->clearAll();
//        ui->text_tosend->clearAll();
//    }

    deselectPort();

    monitored_port_ = port;
    ui->lb_monitoredPort->setText("Monitoring port @ " + monitored_port_->getUUID());

//    ui->out_console->append(QString("Monitoring new port: ") + monitored_port_)

    connection_physical_monitor_ = connect(monitored_port_, &GenericPort::Tx, ui->text_in, &TextDisplay::add_line);
    connection_physical_manual_send_ = connect(ui->text_in, &TextDisplay::send, monitored_port_, &GenericPort::Rx);

    connection_connected_monitor_ = connect(monitored_port_, &GenericPort::dataFromConnection, ui->text_tosend, &TextDisplay::add_line);
    connection_connect_manual_send_ = connect(ui->text_tosend, &TextDisplay::send, monitored_port_, &GenericPort::manualSend);
}

void MainWindow::deselectPort()
{
    if(monitored_port_ != nullptr){
        disconnect(connection_physical_monitor_);
        disconnect(connection_physical_manual_send_);

        disconnect(connection_connected_monitor_);
        disconnect(connection_connect_manual_send_);
    }

    ui->lb_monitoredPort->setText("Monitoring port @ NONE");
    monitored_port_ = nullptr;
}

void MainWindow::consoleLog(const QString &msg)
{
    ui->out_console->append(msg);
}

void MainWindow::setupNewPort(GenericPort *port, const QString &type)
{
    QGraphicsProxyWidget* proxyWidget = ui->out_mainview->scene()->addWidget(port);

    ui->out_console->append(QString("New %1 port added").arg(type));

    connect(port, &GenericPort::destroyed, this, [port, this](){
        dynamic_cast<MyScene *>(ui->out_mainview->scene())->portDeleted(port);
        ports_.removeOne(port);
    });
    connect(ui->actionStart_All, &QAction::triggered, port, &GenericPort::RequestConnect);
    connect(ui->actionStop_All, &QAction::triggered, port, &GenericPort::RequestDisconnect);
    connect(port, &GenericPort::consoleLog, this, [this, port](const QString &msg){
        QString formatted_msg = "[" + port->getUUID() + "] " + msg;
        consoleLog(formatted_msg);
    });

    ports_.push_back(port);
}

int MainWindow::findPort(GenericPort *port)
{
    for(int x = 0; x < ports_.size(); x++){
        if(port == ports_.at(x)){
            return x;
        }
    }

    return -1;
}


void MainWindow::on_actionSave_As_triggered()
{
    auto filename = QFileDialog::getSaveFileName(this,
    tr("Save As"), "/home", tr("Files (*.xml)"));

    if(filename.isEmpty()){
        return;
    }

    if(!filename.contains(".xml")){
        filename += ".xml";
    }

    currentFileName_ = filename;

    this->setWindowTitle("soqt @ " + currentFileName_);

    on_actionSave_triggered();
}


void MainWindow::on_actionSave_triggered()
{
    if(currentFileName_.isEmpty()){
        on_actionSave_As_triggered();
    }

    auto connections = dynamic_cast<MyScene*>(ui->out_mainview->scene())->getConnections();

    srand (time(NULL));

    QString output;

    QXmlStreamWriter stream(&output);
    stream.setAutoFormatting(true);

    stream.writeStartDocument();
    stream.writeStartElement("Scene");

    stream.writeStartElement("Ports");

    std::map<GenericPort *, int> ids;

    for(auto &port:ports_){
        stream.writeStartElement("port");
        stream.writeTextElement("type", port->metaObject()->className());
        ids.insert({port, rand() % 10000});
        stream.writeTextElement("id", QString::number(ids[port]));
        stream.writeStartElement("Rect");

        auto geo = port->geometry();

        stream.writeTextElement("x", QString::number(geo.x()));
        stream.writeTextElement("y", QString::number(geo.y()));
        stream.writeTextElement("width", QString::number(geo.width()));
        stream.writeTextElement("height", QString::number(geo.height()));

        stream.writeEndElement();
        stream.writeStartElement("configuration");
        port->createConfiguration(stream);
        stream.writeEndElement();
        stream.writeEndElement();


    }

    stream.writeEndElement();

    stream.writeStartElement("Connections");

    for(auto &connection:connections){
        stream.writeStartElement("Connection");

        stream.writeStartElement("Direction");
        stream.writeTextElement("from", QString::number(ids[connection.from_]));
        stream.writeTextElement("to", QString::number(ids[connection.to_]));


        stream.writeStartElement("start");
        stream.writeTextElement("x", QString::number(connection.start_pos_.x()));
        stream.writeTextElement("y", QString::number(connection.start_pos_.y()));
        stream.writeEndElement();

        stream.writeStartElement("end");
        stream.writeTextElement("x", QString::number(connection.end_pos_.x()));
        stream.writeTextElement("y", QString::number(connection.end_pos_.y()));
        stream.writeEndElement();

        stream.writeEndElement();


        stream.writeEndElement();
    }

    stream.writeEndElement();
    stream.writeEndElement();

    QFile file(currentFileName_);
    if(file.open(QIODevice::WriteOnly)){
        file.write(output.toUtf8());

        file.close();
        ui->out_console->append("Sucessfully saved");
    }else{
        ui->out_console->append("Error opening file to save");
    }
}


void MainWindow::on_actionOpen_triggered()
{
    auto filename = QFileDialog::getOpenFileName(this,
    tr("Save As"), "/home", tr("Files (*.xml)"));

    if(filename.isEmpty()){
        return;
    }

    dynamic_cast<MyScene*>(ui->out_mainview->scene())->clearConnections();
    for(auto &port:ports_){
        port->on_bu_delete_clicked();
    }

    currentFileName_ = filename;

    this->setWindowTitle("soqt @ " + currentFileName_);

    QFile file(currentFileName_);
    file.open(QIODevice::ReadOnly);

    QXmlStreamReader reader(&file);

    auto ids = addPortsFromFile(reader);

    file.close();

    QFile file2(currentFileName_);
    file2.open(QIODevice::ReadOnly);

    QXmlStreamReader reader2(&file2);

    addConnectionsFromfile(reader2, ids);
}

std::map<QString, GenericPort *> MainWindow::addPortsFromFile(QXmlStreamReader &reader)
{
    std::map<QString, GenericPort*> ids;
    QList<std::pair<GenericPort*, QRect>> geos;
    auto availablePorts = GenericPort::getAvailablePortConstructors();

    if (reader.readNextStartElement()) {
        if(reader.name() == "Scene"){
            while(reader.readNextStartElement()){
                if (reader.name() == "Ports"){
                    while(reader.readNextStartElement()){
                        if(reader.name() == "port"){
                            GenericPort *port = nullptr;
                            QString id;
                            while(reader.readNextStartElement()){
                                if(reader.name() == "type"){
                                    auto type = reader.readElementText();
                                    if(availablePorts.find(type.toStdString()) != availablePorts.end()){
                                        port = availablePorts[type.toStdString()]();
                                        setupNewPort(port, type);
                                    }else{
                                        qErrnoWarning(("Unknown type: " + type).toStdString().c_str() );
                                    }

                                    if(!id.isEmpty()){
                                        ids.insert({id, port});
                                    }
                                }else if(reader.name() == "id"){
                                    id = reader.readElementText();

                                    if(port != nullptr){

                                        ids.insert({id, port});
                                    }
                                }else if(reader.name() == "Rect"){
                                    QRect rect;
                                    while(reader.readNextStartElement()){
                                        if(reader.name() == "x"){
                                            rect.setX(reader.readElementText().toInt());
                                        }else if(reader.name() == "y"){
                                            rect.setY(reader.readElementText().toInt());
                                        }else if(reader.name() == "width"){
                                            rect.setWidth(reader.readElementText().toInt());
                                        }else if(reader.name() == "height"){
                                            rect.setHeight(reader.readElementText().toInt());
                                        }else{
                                            reader.skipCurrentElement();
                                        }
                                    }
                                    if(port != nullptr){
                                        geos.push_back({port, rect});
                                    }
                                }else if(reader.name() == "configuration"){
                                    if(port != nullptr)
                                        port->loadFromConfiguration(reader);
                                }else{
                                    reader.skipCurrentElement();
                                }
                            }
                        }else{
                            reader.skipCurrentElement();
                        }
                    }
                } else if(reader.name() == "Connections"){
                    reader.skipCurrentElement();
                }else {
                    reader.raiseError(QObject::tr("Incorrect file"));
                }
            }
        }
    }else{
        qDebug() << "what";
    }

    dynamic_cast<MyScene*>(ui->out_mainview->scene())->setGeometries(geos);

    return ids;
}

void MainWindow::addConnectionsFromfile(QXmlStreamReader &reader, std::map<QString, GenericPort *> &ids)
{
    QList<Connection> connections;

    if (reader.readNextStartElement()) {
        if(reader.name() == "Scene"){
            while(reader.readNextStartElement()){
                if (reader.name() == "Ports"){
                    reader.skipCurrentElement();
                } else if(reader.name() == "Connections"){
                    while(reader.readNextStartElement()){
                        if(reader.name() == "Connection"){
                            while(reader.readNextStartElement()){
                                if(reader.name() == "Direction"){
                                    Connection connection;
                                    while(reader.readNextStartElement()){
                                        if(reader.name() == "from"){
                                            connection.from_ = ids[reader.readElementText()];
                                        }else if(reader.name() == "to"){
                                            connection.to_ = ids[reader.readElementText()];
                                        }else if(reader.name() == "start"){
                                            QPoint pt;
                                            while(reader.readNextStartElement()){
                                                if(reader.name() == "x"){
                                                    pt.setX(reader.readElementText().toInt());
                                                }else if(reader.name() == "y"){
                                                    pt.setY(reader.readElementText().toInt());
                                                }else{
                                                    reader.skipCurrentElement();
                                                }
                                            }

                                            connection.start_pos_ = pt;
                                        }else if(reader.name() == "end"){
                                            QPoint pt;
                                            while(reader.readNextStartElement()){
                                                if(reader.name() == "x"){
                                                    pt.setX(reader.readElementText().toInt());
                                                }else if(reader.name() == "y"){
                                                    pt.setY(reader.readElementText().toInt());
                                                }else{
                                                    reader.skipCurrentElement();
                                                }
                                            }

                                            connection.end_pos_ = pt;
                                        }else{
                                            reader.skipCurrentElement();
                                        }
                                    }

                                    connections.push_back(connection);
                                }else{
                                    reader.skipCurrentElement();
                                }
                            }
                        }else{
                            reader.skipCurrentElement();
                        }
                    }
                }
            }
        }else {
            qDebug() << "raise error";
            reader.raiseError(QObject::tr("Incorrect file"));
        }
    }else{
        qDebug() << "raise error what";
    }

    if(reader.error()){
        qDebug() << reader.errorString();
    }

    dynamic_cast<MyScene*>(ui->out_mainview->scene())->loadConnections(connections);
}


void MainWindow::on_bu_connectionTool_clicked()
{
//    dynamic_cast<MyScene*>(ui->out_mainview->scene())->setTool(WorkingTool::Connecting);
//    QPixmap pix(tool_to_icon_[WorkingTool::Connecting]);
//    pix = pix.scaled(24,24);
//    QApplication::restoreOverrideCursor();
//    QApplication::setOverrideCursor(QCursor(pix));

    current_selected_tool_ = WorkingTool::Connecting;
}


void MainWindow::on_bu_selectTool_clicked()
{
    dynamic_cast<MyScene*>(ui->out_mainview->scene())->setTool(WorkingTool::Cursor);
    QApplication::restoreOverrideCursor();

    current_selected_tool_ = WorkingTool::Cursor;
}


void MainWindow::on_bu_moveTool_clicked()
{
//    dynamic_cast<MyScene*>(ui->out_mainview->scene())->setTool(WorkingTool::Dragging);
//    QPixmap pix(tool_to_icon_[WorkingTool::Dragging]);
//    pix = pix.scaled(24,24);
//    QApplication::restoreOverrideCursor();
//    QApplication::setOverrideCursor(QCursor(pix));

    current_selected_tool_ = WorkingTool::Dragging;
}


void MainWindow::on_actionClear_All_triggered()
{
//    on_bu_selectTool_clicked();
//    deselectPort();
//    on_bu_
    auto res = QMessageBox::question(this, "Clear All", "Any unsaved changes will be lost, are you sure you wish to continue?",
                                     QMessageBox::Yes|QMessageBox::No);

    if(res == QMessageBox::Yes){
        clearAll();
    }

}


void MainWindow::on_actionNew_triggered()
{
    auto res = QMessageBox::question(this, "Create New", "Any unsaved changes will be lost, are you sure you wish to continue?",
                                     QMessageBox::Yes|QMessageBox::No);

    if(res == QMessageBox::Yes){
        clearAll();
        on_actionSave_As_triggered();
    }else{
        return;
    }
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    event->accept();
    if(event->type() == QEvent::Leave){
        dynamic_cast<MyScene*>(ui->out_mainview->scene())->setTool(WorkingTool::Cursor);
        QApplication::restoreOverrideCursor();
    }

    if(event->type() == QEvent::Enter){
        if(current_selected_tool_ == WorkingTool::Cursor){
            return false;
        }
        dynamic_cast<MyScene*>(ui->out_mainview->scene())->setTool(current_selected_tool_);
        QPixmap pix(tool_to_icon_[current_selected_tool_]);
        pix = pix.scaled(24,24);
        QApplication::restoreOverrideCursor();
        QApplication::setOverrideCursor(QCursor(pix));
    }

    return false;
}

void MainWindow::clearAll()
{
    dynamic_cast<MyScene*>(ui->out_mainview->scene())->clearConnections();
    dynamic_cast<MyScene*>(ui->out_mainview->scene())->reset();
    on_bu_selectTool_clicked();


    for(auto &port:ports_){
        port->on_bu_delete_clicked();
    }
}

void MainWindow::on_actionToggle_Monitor_triggered()
{
    if(ui->port_monitor_container->isHidden()){
        ui->port_monitor_container->show();
    }else{
        ui->port_monitor_container->hide();
    }
}


void MainWindow::on_actionToggle_Designer_triggered()
{
    if(ui->mainview_container->isHidden()){
        ui->mainview_container->show();
    }else{
        ui->mainview_container->hide();
    }
}


void MainWindow::on_actionToggle_Console_triggered()
{
    if(ui->console_container->isHidden()){
        ui->console_container->show();
    }else{
        ui->console_container->hide();
    }
}

