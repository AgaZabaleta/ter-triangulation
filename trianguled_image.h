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
    void saliency(double gradient_value, double color_value, double texture_value, bool recurrent);
    QImage gradient_saliency();
    QImage color_saliency();
    QImage texture_saliency(bool recurrent);
    void triangulate();
    void addRandomPoint();
    void addPoints();

    QImage getImage();
    std::vector<QPointF*> getPoints();
    std::vector<Triangle*> getTriangles();
    double getScaleX();
    double getScaleY();
    void setN_xy(double);
    QColor getPointColor(int i);
    QColor getTriangleColor(int p1, int p2, int p3);
protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QImage image;
    QImage backupImage; // Pour passer de gris à couleur
    QImage triangles;
    std::vector<QPointF*> points;
    std::vector<QPointF*> points_step;
    std::vector<QPointF*> points_final;
    std::vector<Triangle*> tab_triangles;
    std::vector<std::vector<int>> neighbours;
    bool point_moved;
    int max_size = 500;
    void resizeImage(QImage *image, const QSize &newSize);

    int n_y;
    int n_x;
    int vision_range;
    void addNeighbor(int i, int j);
    QPointF getBarycenter(int i);

    void setVision_range();
    bool triangulate_step();
    QPoint getBestPoint(QPoint point);

    int getPointValue(QPoint point);
    QPoint getNextPoint(QPoint p_origin, QPoint p_best);
};

#endif // TRIANGULED_IMAGE_H
