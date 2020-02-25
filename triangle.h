#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <QtWidgets>

class Triangle
{
public:
    Triangle(int p1, int p2, int p3);
//    void draw_triangle(QPainter* painter);
private:
    int p1;
    int p2;
    int p3;
};

#endif // TRIANGLE_H
