#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <QtWidgets>

class Triangle
{
public:
    Triangle(QPoint* p1, QPoint* p2, QPoint* p3);
    void draw_triangle(QPainter* painter);
private:
    QPoint* p1;
    QPoint* p2;
    QPoint* p3;
};

#endif // TRIANGLE_H
