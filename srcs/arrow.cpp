#include "arrow.h"

Arrow::Arrow(QPoint pt1, QPoint pt2): pt1_(pt1), pt2_(pt2)
{

}

void Arrow::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHint(QPainter::Antialiasing, true);

    qreal arrowSize = 40; // size of head
    painter->setPen(Qt::black);
    painter->setBrush(Qt::black);

    QLineF line(pt2_, pt1_);

    double angle = std::atan2(-line.dy(), line.dx());
    QPointF arrowP1 = line.p1() + QPointF(sin(angle + M_PI / 3) * arrowSize,
                                        cos(angle + M_PI / 3) * arrowSize);
    QPointF arrowP2 = line.p1() + QPointF(sin(angle + M_PI - M_PI / 3) * arrowSize,
                                        cos(angle + M_PI - M_PI / 3) * arrowSize);

    QPolygonF arrowHead;
    arrowHead.clear();
    arrowHead << line.p1() << arrowP1 << arrowP2;
    painter->drawLine(line);
    painter->drawPolygon(arrowHead);

//    QGraphicsItem::paint(painter, option);
}

QRectF Arrow::boundingRect() const
{

}
