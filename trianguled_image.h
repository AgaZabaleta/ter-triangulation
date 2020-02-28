#ifndef TRIANGULED_IMAGE_H
#define TRIANGULED_IMAGE_H

#include <vector>
#include <QWidget>
#include <math.h>

#include "triangle.h"

class Trianguled_image : public QWidget
{
    Q_OBJECT
public:
    Trianguled_image(int n_columns, int n_rows, QWidget *parent = nullptr);
    bool openImage(const QString &fileName);
    bool saveImage(const QString &fileName, const char *fileFormat);
    void transformToGrey();
    void triangulate();
    void addRandomPoint();
    void addPoints();
    std::vector<QPointF*> getPoints();
    std::vector<Triangle*> getTriangles();
protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QImage image;
    QImage backupImage; // Pour passer de gris à couleur
    QImage triangles;
    std::vector<QPointF*> points;
    std::vector<Triangle*> tab_triangles;
    bool point_moved;
    int max_size = 500;
    void resizeImage(QImage *image, const QSize &newSize);

    int n_y;
    int n_x;
    bool triangulate_step();
    QPoint getBestPoint(QPoint point);
    int vision_range = 50;
    int getPointValue(QPoint point);
    QPoint getNextPoint(QPoint p_origin, QPoint p_best);
};

#endif // TRIANGULED_IMAGE_H
