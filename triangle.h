#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <QtWidgets>

class Triangle
{
public:
    Triangle(int p1, int p2, int p3);
//    void draw_triangle(QPainter* painter);
    int getP1() const;
    void setP1(int value);

    int getP2() const;
    void setP2(int value);

    int getP3() const;
    void setP3(int value);

private:
    int p1;
    int p2;
    int p3;
};

#endif // TRIANGLE_H
