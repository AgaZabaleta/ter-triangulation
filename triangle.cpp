#include "triangle.h"

Triangle::Triangle(int point1, int point2, int point3) : p1(point1), p2(point2), p3(point3)
{
}

int Triangle::getP1() const
{
    return p1;
}

void Triangle::setP1(int value)
{
    p1 = value;
}

int Triangle::getP2() const
{
    return p2;
}

void Triangle::setP2(int value)
{
    p2 = value;
}

int Triangle::getP3() const
{
    return p3;
}

void Triangle::setP3(int value)
{
    p3 = value;
}

//void Triangle::draw_triangle(QPainter* painter)
//{
//    qInfo() << "Point drawn : " << p1->x() << " " << p1->y();
//    qInfo() << "Point drawn : " << p2->x() << " " << p2->y();
//    qInfo() << "Point drawn : " << p3->x() << " " << p3->y();

//    painter->drawLine(*p1, *p2);
//    painter->drawLine(*p2, *p3);
//    painter->drawLine(*p3, *p1);

//    painter->drawPoint(*p1);
//    painter->drawPoint(*p2);
//    painter->drawPoint(*p3);
//}
