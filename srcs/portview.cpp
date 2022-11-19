#include "portview.h"

PortView::PortView(QWidget *parent): QGraphicsView(parent)
{
//    middle_button_ = false;
//    cntrl_pressed_ = false;
}

//void PortView::fitSceneInView()
//{
//    this->fitInView(this->scene()->sceneRect());
//}

//void PortView::scalingTime(qreal x)
//{
//    qreal factor = 1.0+ qreal(_numScheduledScalings) / 300.0;
//    scale(factor, factor);
//}

//void PortView::animFinished()
//{
//    if (_numScheduledScalings > 0)
//    _numScheduledScalings--;
//    else
//    _numScheduledScalings++;
//    sender()->~QObject();
//}

//void PortView::resizeEvent(QResizeEvent *event)
//{
//    QGraphicsView::resizeEvent(event);
//}

//void PortView::wheelEvent(QWheelEvent *event)
//{
//    if(cntrl_pressed_){
//        event->accept();
//        int numDegrees = event->delta() / 8;
//        int numSteps = numDegrees / 15; // see QWheelEvent documentation
//        _numScheduledScalings += numSteps;
//        if (_numScheduledScalings * numSteps < 0) // if user moved the wheel in another direction, we reset previously scheduled scalings
//        _numScheduledScalings = numSteps;

//        QTimeLine *anim = new QTimeLine(350, this);
//        anim->setUpdateInterval(20);

//        connect(anim, &QTimeLine::valueChanged, this, &PortView::scalingTime);
//        connect(anim, &QTimeLine::finished, this, &PortView::animFinished);
//        anim->start();
//    }

//    QGraphicsView::wheelEvent(event);
//}

//void PortView::mouseMoveEvent(QMouseEvent *event)
//{
//    if(middle_button_){
//        QPointF oldp = mapToScene(m_originX, m_originY);
//        QPointF newp = mapToScene(event->pos());
//        QPointF translation = newp - oldp;

//        translate(translation.x(), translation.y());

//        m_originX = event->x();
//        m_originY = event->y();
//    }

//    QGraphicsView::mouseMoveEvent(event);
//}

//void PortView::mousePressEvent(QMouseEvent *event)
//{
//    if (event->button() == Qt::MiddleButton){
//        middle_button_ = true;
//        // Store original position.
//        m_originX = event->x();
//        m_originY = event->y();
//    }

//    QGraphicsView::mousePressEvent(event);
//}

//void PortView::mouseReleaseEvent(QMouseEvent *event)
//{
//    if (event->button() == Qt::MiddleButton){
//        middle_button_ = false;
//    }

//    QGraphicsView::mouseReleaseEvent(event);
//}

//void PortView::keyPressEvent(QKeyEvent *keyEvent)
//{
//    if(keyEvent->key() == Qt::Key_Control){
//        cntrl_pressed_= true;
//    }
//    QGraphicsView::keyPressEvent(keyEvent);
//}

//void PortView::keyReleaseEvent(QKeyEvent *keyEvent)
//{
//    if(keyEvent->key() == Qt::Key_Control){
//        cntrl_pressed_= false;
//    }
//    QGraphicsView::keyReleaseEvent(keyEvent);
//}
