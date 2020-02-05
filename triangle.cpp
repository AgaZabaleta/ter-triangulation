#include "triangle.h"

Triangle::Triangle(QPoint* point1, QPoint* point2, QPoint* point3) : p1(point1), p2(point2), p3(point3)
{
}

void Triangle::draw_triangle(QPainter* painter)
{
    qInfo() << "Point drawn : " << p1->x() << " " << p1->y();
    qInfo() << "Point drawn : " << p2->x() << " " << p2->y();
    qInfo() << "Point drawn : " << p3->x() << " " << p3->y();

    painter->drawLine(*p1, *p2);
    painter->drawLine(*p2, *p3);
    painter->drawLine(*p3, *p1);

    painter->drawPoint(*p1);
    painter->drawPoint(*p2);
    painter->drawPoint(*p3);
}
