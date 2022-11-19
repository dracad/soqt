#ifndef ARROW_H
#define ARROW_H

#include <QGraphicsItem>
#include <QPoint>
#include <QPen>
#include <QPainter>

#include <math.h>

class Arrow : public QGraphicsItem
{
public:
    Arrow(QPoint pt1, QPoint pt2);

private:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr);
    QRectF boundingRect() const;

    QPoint pt1_;
    QPoint pt2_;
};

#endif // ARROW_H
