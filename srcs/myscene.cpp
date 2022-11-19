#include "myscene.h"

MyScene::MyScene(QObject *parent)
    : QGraphicsScene{parent}
{
    dragging_       = false;
    cntrl_pressed_  = false;
    drawing_        = false;

    current_dragged_object = nullptr;
    current_selected_port_ = nullptr;
    temp_line_             = nullptr;
    potential_end_port_    = nullptr;
    selected_line_         = nullptr;

    selected_tool_ = WorkingTool::Cursor;

//    setBackgroundBrush(Qt::lightGray);
}

void MyScene::loadConnections(QList<Connection> connections)
{
    connections_ = connections;
    for(auto &con:connections_){
        con.connection_ = connect(con.from_, &GenericPort::Tx, con.to_, &GenericPort::Rx);
        auto pt1 = widgeToScene(con.from_, con.start_pos_);
        auto pt2 = widgeToScene(con.to_, con.end_pos_);
        con.ownedLine_ = drawLine(pt1, pt2, QColor(0,0,0), 5);
    }
}

void MyScene::clearConnections()
{
    for(auto &connection:connections_){
        disconnect(connection.connection_);
        delete connection.ownedLine_;
    }

    connections_.clear();
}

void MyScene::setGeometries(QList<std::pair<GenericPort *, QRect> > geos)
{
    for(auto &geo:geos){
        geo.first->setGeometry(geo.second);
    }
}

void MyScene::reset()
{
    deselect_port();
    selected_tool_ = WorkingTool::Cursor;

    if(temp_line_ != nullptr){
        delete temp_line_;
        temp_line_ = nullptr;
    }

    if(selected_line_ != nullptr){
        selected_line_->graphicsEffect()->deleteLater();
        selected_line_ = nullptr;
    }

    current_dragged_object = nullptr;
    if(potential_end_port_ != nullptr){
        potential_end_port_->tempDeselected();
        potential_end_port_ = nullptr;
    }
}

void MyScene::portDeleted(GenericPort *port)
{
    auto connections = getAllConnectionsContaining(port);

    for(auto &connection:connections){
        disconnect(connection.connection_);
        delete connection.ownedLine_;

        removeFromList(connections_, connection);
    }
}

void MyScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(dragging_){
        mouseEvent->accept();
        auto startingConnections = getAllConnectionsThatStartWith(dynamic_cast<GenericPort*>(current_dragged_object));
        auto endingConnections = getAllConnectionsThatEndWith(dynamic_cast<GenericPort*>(current_dragged_object));

        current_dragged_object->setGeometry(mouseEvent->scenePos().x(), mouseEvent->scenePos().y(), current_dragged_object->width(), current_dragged_object->height());

        for(auto &connection:startingConnections){
            auto pt1 = widgeToScene(connection.from_, connection.start_pos_);
            auto pt2 = widgeToScene(connection.to_, connection.end_pos_);
            delete connection.ownedLine_;
            connection.ownedLine_ = drawLine(pt1, pt2, QColor(0,0,0), 5);
        }

        for(auto &connection:endingConnections){
            auto pt1 = widgeToScene(connection.from_, connection.start_pos_);
            auto pt2 = widgeToScene(connection.to_, connection.end_pos_);
            delete connection.ownedLine_;
            connection.ownedLine_ = drawLine(pt1, pt2, QColor(0,0,0), 5);
        }
    }else if(drawing_){
        mouseEvent->accept();
        auto item = itemAt(mouseEvent->scenePos(), QTransform());

        if(item != nullptr && item->type() == 12){
            if(current_selected_port_ == dynamic_cast<QGraphicsProxyWidget *>(item)->widget()){
                QPoint mousePos = current_selected_port_->mapFromGlobal(mouseEvent->screenPos());

                start_pos_ = pointToEdge(current_selected_port_, mousePos);

                start_pos_widge_ = start_pos_;

                start_pos_ = widgeToScene(current_selected_port_, start_pos_);

                if(potential_end_port_ != nullptr){
                    potential_end_port_->tempDeselected();
                    potential_end_port_ = nullptr;
                }
            }else{
                auto temp = dynamic_cast<GenericPort *>(dynamic_cast<QGraphicsProxyWidget *>(item)->widget());
                if(!temp->supportsRx()){
                    return;
                }
                auto tempConnect = connect(current_selected_port_, &GenericPort::Tx, temp, &GenericPort::Rx, Qt::UniqueConnection);

                if(!tempConnect){
                    return;
                }

                disconnect(tempConnect);

                potential_end_port_ = temp;
                potential_end_port_->tempSelected();

                QPoint mousePos = potential_end_port_->mapFromGlobal(mouseEvent->screenPos());

                end_pos_ = pointToEdge(potential_end_port_, mousePos);

                end_pos_widge_ = end_pos_;

                end_pos_ = widgeToScene(potential_end_port_, end_pos_);

                drawTempLine(start_pos_, end_pos_, QColor(255,0,0), 5);
            }
        }else{
            mouseEvent->accept();
            if(potential_end_port_ != nullptr){
                potential_end_port_->tempDeselected();
                potential_end_port_ = nullptr;
            }

            drawTempLine(start_pos_, mouseEvent->scenePos().toPoint(), QColor(255,0,0), 5);
        }
    }

    QGraphicsScene::mouseMoveEvent(mouseEvent);
}

void MyScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(mouseEvent->button() == Qt::LeftButton && (selected_tool_ == WorkingTool::Connecting)){
        auto item = itemAt(mouseEvent->scenePos(), QTransform());

        if(item != nullptr && item->type() == 12){
            auto port = dynamic_cast<QGraphicsProxyWidget *>(item)->widget();
            auto genport = dynamic_cast<GenericPort *>(port);
            if(port->objectName().compare("GenericPort") != 0){
                return;
            }
            select_port(port);

            if(genport->supportsTx()){
                drawing_ = true;
            }

        }else if(item != nullptr && item->type() != 12){
            qDebug() << "Cntrl Selected an item";
        }
    }else if(mouseEvent->button() == Qt::LeftButton && (selected_tool_ == WorkingTool::Cursor)){
        auto item = itemAt(mouseEvent->scenePos(), QTransform());
        if(item != nullptr && item->type() == 12){
            auto proxy = dynamic_cast<QGraphicsProxyWidget *>(item);
            auto widge = proxy->widget();

            if(widge->objectName().compare("GenericPort") != 0){
                return;
            }

            select_port(widge);
        }else if(item != nullptr && item->type() != 12){
            if(selected_line_ != nullptr){
                selected_line_->graphicsEffect()->deleteLater();
            }

            selected_line_ = item;
            item->setGraphicsEffect(new QGraphicsColorizeEffect());
            deselect_port();
        }else{
            if(selected_line_ != nullptr){
                selected_line_->graphicsEffect()->deleteLater();
                selected_line_ = nullptr;
            }

            deselect_port();
        }
    }else if(mouseEvent->button() == Qt::LeftButton && (selected_tool_ == WorkingTool::Dragging)){
        auto item = itemAt(mouseEvent->scenePos(), QTransform());

        if(item != nullptr && item->type() == 12){
            auto proxy = dynamic_cast<QGraphicsProxyWidget *>(item);
            auto widge = proxy->widget();

            if(widge->objectName().compare("GenericPort") != 0){
                return;
            }

            current_dragged_object = widge;
            dragging_ = true;

            select_port(current_dragged_object);

        }
    }

    QGraphicsScene::mousePressEvent(mouseEvent);
}

void MyScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    mouseEvent->accept();
    if(mouseEvent->button() == Qt::LeftButton){
        dragging_ = false;
        current_dragged_object = nullptr;

        if(drawing_){
            drawing_ = false;

            if(current_selected_port_ && potential_end_port_ && temp_line_){
                auto setLine = drawLine(start_pos_, end_pos_, QColor(0,0,0), 5);
                delete temp_line_;
                temp_line_ = nullptr;

                auto res = connect(current_selected_port_, &GenericPort::Tx, potential_end_port_, &GenericPort::Rx, Qt::UniqueConnection);

                if(res){
                    Connection connection;
                    connection.from_ = current_selected_port_;
                    connection.to_   = potential_end_port_;
                    connection.ownedLine_ = setLine;
                    connection.start_pos_ = start_pos_widge_;
                    connection.end_pos_   = end_pos_widge_;
                    connection.connection_ = res;

                    connections_.push_back(connection);

                    emit consoleLog(QStringLiteral("Successfully connected"));
                }else{
                    emit consoleLog(QStringLiteral("Connection Failed"));
                }

                potential_end_port_->tempDeselected();
                potential_end_port_ = nullptr;

            }else if(temp_line_){
                delete temp_line_;
                temp_line_ = nullptr;
            }
        }
    }
//    mouseEvent->accept();
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

void MyScene::keyPressEvent(QKeyEvent *keyEvent)
{
    if(keyEvent->key() == Qt::Key_Control){
        cntrl_pressed_= true;
    }
    QGraphicsScene::keyPressEvent(keyEvent);
}

void MyScene::keyReleaseEvent(QKeyEvent *keyEvent)
{
    if(keyEvent->key() == Qt::Key_Control){
        cntrl_pressed_= false;
    }else if(keyEvent->key() == Qt::Key_Delete){
        if(selected_line_){
            auto connection = getConnection(selected_line_);
            if(connection.ownedLine_){
                delete selected_line_;
                selected_line_ = nullptr;
                disconnect(connection.connection_);
                removeFromList(connections_, connection);
            }
        }
//        if(current_selected_port_ != nullptr){
////            current_selected_port_->deleteLater();
//            delete current_selected_port_;
//            current_selected_port_ = nullptr;
//        }
    }
    QGraphicsScene::keyReleaseEvent(keyEvent);
}

void MyScene::select_port(QWidget *port)
{
    deselect_port();

    if(selected_line_ != nullptr){
        selected_line_->graphicsEffect()->deleteLater();
        selected_line_ = nullptr;
    }

    current_selected_port_ = dynamic_cast<GenericPort *>(port);
    current_selected_port_->selected();
    emit portSelected(current_selected_port_);

    connect(current_selected_port_, &GenericPort::destroyed, this, [this](){current_selected_port_ = nullptr; emit portDeselected();});
}

void MyScene::deselect_port()
{
    if(current_selected_port_ != nullptr){
        current_selected_port_->deselected();
        emit portDeselected();
    }

    current_selected_port_ = nullptr;
}

QPoint MyScene::pointToEdge(QWidget *widge, const QPoint &pointInWidge)
{
    /* Quadrants
     *
     * (0,0)
     *       2 | 1
     *      -------
     *       3 | 4
     *
     */

    QPoint pos;
    if(pointInWidge.x() > widge->width()/2 && pointInWidge.y() < widge->height()/2){ //Quad 1
        auto distance_to_edge_X = static_cast<double>(widge->width() - pointInWidge.x())/static_cast<double>(widge->width());
        auto distance_to_edge_Y = static_cast<double>(pointInWidge.y())/static_cast<double>(widge->height());

        if(distance_to_edge_X < distance_to_edge_Y){
            pos.setX(widge->width());
            pos.setY(pointInWidge.y());
        }else{
            pos.setX(pointInWidge.x());
            pos.setY(0);
        }
    }else if(pointInWidge.x() < widge->width()/2 && pointInWidge.y() < widge->height()/2){//Quad 2
        auto distance_to_edge_X = static_cast<double>(pointInWidge.x())/static_cast<double>(widge->width());
        auto distance_to_edge_Y = static_cast<double>(pointInWidge.y())/static_cast<double>(widge->height());

        if(distance_to_edge_X < distance_to_edge_Y){
            pos.setX(0);
            pos.setY(pointInWidge.y());
        }else{
            pos.setX(pointInWidge.x());
            pos.setY(0);
        }
    }else if(pointInWidge.x() < widge->width()/2 && pointInWidge.y() > widge->height()/2){//Quad 3
        auto distance_to_edge_X = static_cast<double>(pointInWidge.x())/static_cast<double>(widge->width());
        auto distance_to_edge_Y = static_cast<double>(widge->height() - pointInWidge.y())/static_cast<double>(widge->height());

        if(distance_to_edge_X < distance_to_edge_Y){
            pos.setX(0);
            pos.setY(pointInWidge.y());
        }else{
            pos.setX(pointInWidge.x());
            pos.setY(widge->height());
        }

    }else if(pointInWidge.x() > widge->width()/2 && pointInWidge.y() > widge->height()/2){//Quad 4
        auto distance_to_edge_X = static_cast<double>(widge->width() - pointInWidge.x())/static_cast<double>(widge->width());
        auto distance_to_edge_Y = static_cast<double>(widge->height() - pointInWidge.y())/static_cast<double>(widge->height());

        if(distance_to_edge_X < distance_to_edge_Y){
            pos.setX(widge->width());
            pos.setY(pointInWidge.y());
        }else{
            pos.setX(pointInWidge.x());
            pos.setY(widge->height());
        }
    }

    return pos;
}

QPoint MyScene::widgeToScene(QWidget *widge, const QPoint &pt)
{
    QPoint pos;
    pos.setX(widge->geometry().x() + pt.x());
    pos.setY(widge->geometry().y() + pt.y());

    return pos;
}

QPoint MyScene::sceneToWidge(QWidget *widge, const QPoint &pt)
{
    QPoint pos;
    pos.setX(pt.x() - widge->geometry().x());
    pos.setY(pt.y() - widge->geometry().y());

    return pos;
}

void MyScene::removeFromList(QList<Connection> &list, const Connection &connection)
{
    for(auto it = list.begin(); it != list.end(); it++){
        if(it->to_ == connection.to_ && it->from_ == connection.from_){
            list.erase(it);
            return;
        }
    }
}

QList<Connection> MyScene::getAllConnectionsThatStartWith(GenericPort *port)
{
    QList<Connection> connections;

    for(auto &connection:connections_){
        if(connection.from_ == port){
            connections.push_back(connection);
        }
    }

    return connections;
}

QList<Connection> MyScene::getAllConnectionsThatEndWith(GenericPort *port)
{
    QList<Connection> connections;

    for(auto &connection:connections_){
        if(connection.to_ == port){
            connections.push_back(connection);
        }
    }

    return connections;
}

QList<Connection> MyScene::getAllConnectionsContaining(GenericPort *port)
{
    QList<Connection> connections;

    for(auto &connection:connections_){
        if(connection.to_ == port || connection.from_ == port){
            connections.push_back(connection);
        }
    }

    return connections;
}

Connection MyScene::getConnection(QGraphicsItem *item)
{
    for(auto &connection:connections_){
        if(connection.ownedLine_ == item){
            return connection;
        }
    }

    Connection conn;
    return conn;
}

QGraphicsItem *MyScene::drawLine(const QPoint &pt1, const QPoint &pt2, QColor c, int thickness)
{
//    auto body = this->addLine(pt1.x(), pt1.y(), pt2.x(), pt2.y(), QPen(c, thickness));

    qreal arrowSize = 20; // size of head

    QLineF line(pt2, pt1);

    double angle = std::atan2(-line.dy(), line.dx());
    QPointF arrowP1 = line.p1() + QPointF(sin(angle + M_PI / 3) * arrowSize,
                                        cos(angle + M_PI / 3) * arrowSize);
    QPointF arrowP2 = line.p1() + QPointF(sin(angle + M_PI - M_PI / 3) * arrowSize,
                                        cos(angle + M_PI - M_PI / 3) * arrowSize);

    QPolygonF arrowHead;
    arrowHead.clear();
    arrowHead << line.p1() << arrowP1 << arrowP2;

//    auto head = this->addPolygon(arrowHead, QPen(c, thickness), QBrush(c));

    auto arrow = new QGraphicsItemGroup();
    auto body = new QGraphicsLineItem(pt1.x(), pt1.y(), pt2.x(), pt2.y());
    auto head = new QGraphicsPolygonItem(arrowHead);

    body->setPen(QPen(c, thickness));
    head->setPen(QPen(c, thickness));
    head->setBrush(QBrush(c));

    arrow->addToGroup(body);
    arrow->addToGroup(head);
    arrow->setZValue(-1);

    this->addItem(arrow);

    return arrow;


//    body->setZValue(-1);
//    head->setZValue(-1);
//    head->setParentItem(body);

//    return body;
}

void MyScene::drawTempLine(const QPoint &pt1, const QPoint &pt2, QColor c, int thickness)
{
    if(temp_line_ != nullptr){
        delete temp_line_;
        temp_line_ = nullptr;
    }

    temp_line_ = drawLine(pt1, pt2, c, thickness);
}
