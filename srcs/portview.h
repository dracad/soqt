#ifndef PORTVIEW_H
#define PORTVIEW_H

#include <QGraphicsView>
#include <QMouseEvent>
#include <QDrag>
#include <QTimeLine>

#include <QDebug>

class PortView : public QGraphicsView
{
    Q_OBJECT
public:
    PortView(QWidget *parent = nullptr);

//public slots:
//    void fitSceneInView();

//private slots:
//    void scalingTime(qreal x);
//    void animFinished();

//private:
//    void resizeEvent(QResizeEvent *event);
//    void wheelEvent ( QWheelEvent * event );
//    void mouseMoveEvent(QMouseEvent* event);
//    void mousePressEvent(QMouseEvent* event);
//    void mouseReleaseEvent(QMouseEvent* event);
//    void keyPressEvent(QKeyEvent *keyEvent);
//    void keyReleaseEvent(QKeyEvent *keyEvent);

//    int _numScheduledScalings;

//    bool middle_button_;
//    bool cntrl_pressed_;

//    int m_originX;
//    int m_originY;

};

#endif // PORTVIEW_H
