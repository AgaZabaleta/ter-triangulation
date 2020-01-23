#ifndef TRIANGULED_IMAGE_H
#define TRIANGULED_IMAGE_H

#include <vector>
#include <QWidget>
#include <math.h>

class Trianguled_image : public QWidget
{
    Q_OBJECT
public:
    Trianguled_image(QWidget *parent = nullptr);
    bool openImage(const QString &fileName);
    bool saveImage(const QString &fileName, const char *fileFormat);
    void transformToGrey();
    void triangulate();
    void addRandomPoint();
    void addPoints();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QImage image;
    QImage backupImage; // Pour passer de gris à couleur
    QImage triangles;
    std::vector<QPoint*> points;
    bool point_moved;
    int max_size = 500;
    void resizeImage(QImage *image, const QSize &newSize);

    bool triangulate_step();
    QPoint getBestPoint(QPoint point);
    int vision_range = 50;
    int getPointValue(QPoint point);
};

#endif // TRIANGULED_IMAGE_H
