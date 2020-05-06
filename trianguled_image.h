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
    double getTriangleError(int p1, int p2, int p3, std::vector<QPointF*> &source_points);
    double getPointError(int curr_point, std::vector<QPointF*> &source_points);
protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QImage image;
    QImage saliencyMap;
    QImage backupImage; // Pour passer de gris à couleur
    QImage triangles;
    std::vector<QPointF*> points;
    std::vector<QPointF*> points_buffer;
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
    int cardinal_range = 10; // deplacement triangle error
    void addNeighbor(int i, int j);
    void addAdjacentTriangle(int i, int triangle);
    QPointF getBarycenter(int i, std::vector<QPointF*> &source_points);

    void setVision_range();
    bool triangulate_step();
    void step_saliency(std::vector<QPointF*> &source_points, std::vector<QPointF*> &dest_points);
    void step_error(std::vector<QPointF*> &source_points, std::vector<QPointF*> &dest_points);
    void step_laplacian(std::vector<QPointF*> &source_points, std::vector<QPointF*> &dest_points, float weight);
    double laplacian_dotproduct(int i, std::vector<QPointF*> &source_points);
    QPoint getBestPoint(QPoint point);

    int getPointValue(QPoint point);
    QPoint getNextPoint(QPoint p_origin, QPoint p_best);

    // Test settings //
    QString testImage = "C:/Users/juj/Desktop/rond1.png";
    double percent_test = 0.05;
    double gradient_value_test = 1.0;
    double color_value_test = 0.0;
    double texture_value_test = 0.0;
    double laplacian_value_test = 0.0;
    bool recurrent_value_test = false;
    // Test settings //

};

#endif // TRIANGULED_IMAGE_H
