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
    void switchImage();
    void saliency(double gradient_value, double color_value, double texture_value, bool recurrent);
    QImage gradient_saliency();
    QImage color_saliency();
    QImage texture_saliency(bool recurrent);
    void triangulate();
    void addPoints();
    void test();
    void testSettings(double p, double gv, double cv, double tv, bool rec, double lp);
    void reset();

    QImage getImage();
    std::vector<QPointF*> getPoints();
    std::vector<Triangle*> getTriangles();
    double getScaleX();
    double getScaleY();
    void setN_xy(double percent=0.05);
    QColor getPointColor(int i);
    QColor getTriangleColor(int p1, int p2, int p3);
    double getTriangleError(int p1, int p2, int p3);
    double getPointError(int curr_point);
protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QImage image;
    QImage saliencyMap;
    QImage backupImage; // Pour passer de gris à couleur
    QImage triangles;
    std::vector<QPointF*> points;
    std::vector<QPointF*> points_step;
    std::vector<QPointF*> points_final;
    std::vector<Triangle*> tab_triangles;
    std::vector<std::vector<int>> neighbours;
    std::vector<std::vector<int>> adjacent_triangles;
    bool point_moved;
    int max_size = 500;
    void resizeImage(QImage *image, const QSize &newSize);

    int n_y;
    int n_x;
    int vision_range;
    void addNeighbor(int i, int j);
    void addAdjacentTriangle(int i, int triangle);
    QPointF getBarycenter(int i);

    void setVision_range();
    bool triangulate_step();
    void laplacian_smoothing(float weight);
    double laplacian_dotproduct(int i);
    QPoint getBestPoint(QPoint point);

    int getPointValue(QPoint point);
    QPoint getNextPoint(QPoint p_origin, QPoint p_best);

    // Test settings //
    QString testImage = "C:/Users/basti/OneDrive/Bureau/panneau_stop.jpg";
    double percent_test = 0.05;
    double gradient_value_test = 1.0;
    double color_value_test = 0.0;
    double texture_value_test = 0.0;
    double laplacian_value_test = 0.01;
    bool recurrent_value_test = false;
    // Test settings //

};

#endif // TRIANGULED_IMAGE_H
