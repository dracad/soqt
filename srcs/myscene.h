#ifndef MYSCENE_H
#define MYSCENE_H

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QDebug>
#include <QWidget>
#include <QGraphicsItem>
#include <QGraphicsProxyWidget>
#include <QKeyEvent>
#include <QPoint>
#include <QGraphicsSvgItem>
#include <QGraphicsItemGroup>
#include <QGraphicsColorizeEffect>

#include <srcs/arrow.h>
#include <ports/genericport.h>

#include <math.h>
struct Connection
{
    GenericPort *from_ = nullptr;
    GenericPort *to_ = nullptr;

    QGraphicsItem *ownedLine_ = nullptr;
    QPoint start_pos_;
    QPoint end_pos_;

    QMetaObject::Connection connection_;
};

enum WorkingTool {
    Dragging = 1,
    Connecting = 2,
    Cursor = 3
};

class MyScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit MyScene(QObject *parent = nullptr);

    QList<Connection> getConnections(){return connections_;}
    void loadConnections(QList<Connection> connections);
    void clearConnections();
    void setGeometries(QList<std::pair<GenericPort*, QRect>> geos);
    void setTool(WorkingTool tool){selected_tool_ = tool;}
    void reset();

public slots:
    void portDeleted(GenericPort *port);

private:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void keyPressEvent(QKeyEvent *keyEvent);
    void keyReleaseEvent(QKeyEvent *keyEvent);

    void select_port(QWidget *port);
    void deselect_port();
    void drawTempLine(const QPoint &pt1, const QPoint &pt2, QColor c, int thickness);
    QGraphicsItem *drawLine(const QPoint &pt1, const QPoint &pt2, QColor c, int thickness);

    static QPoint pointToEdge(QWidget *widge, const QPoint &pointInWidge);
    static QPoint widgeToScene(QWidget *widge, const QPoint &pt);
    static QPoint sceneToWidge(QWidget *widge, const QPoint &pt);
    static void removeFromList(QList<Connection> &list, const Connection &connection);

    QList<Connection> getAllConnectionsThatStartWith(GenericPort *port);
    QList<Connection> getAllConnectionsThatEndWith(GenericPort *port);
    QList<Connection> getAllConnectionsContaining(GenericPort *port);

    Connection getConnection(QGraphicsItem *item);

private:
    bool dragging_;
    bool cntrl_pressed_;
    bool drawing_;

    WorkingTool selected_tool_;

    QGraphicsItem *temp_line_;
    QGraphicsItem *selected_line_;

    QWidget *current_dragged_object;
    GenericPort *current_selected_port_;
    GenericPort *potential_end_port_;

    QPoint start_pos_;
    QPoint start_pos_widge_;
    QPoint end_pos_;
    QPoint end_pos_widge_;

    QList<Connection> connections_;

signals:
    void consoleLog(QString msg);
    void portSelected(GenericPort *port);
    void portDeselected();

};

#endif // MYSCENE_H
