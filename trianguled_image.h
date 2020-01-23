#ifndef TRIANGULED_IMAGE_H
#define TRIANGULED_IMAGE_H

#include <vector>
#include <QWidget>

class Trianguled_image : public QWidget
{
    Q_OBJECT
public:
    Trianguled_image(QWidget *parent = nullptr);
    bool openImage(const QString &fileName);
    bool saveImage(const QString &fileName, const char *fileFormat);
    void triangulate();
    void addRandomPoint();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QImage image;
    QImage triangles;
    std::vector<QPoint*> points;
    bool point_moved;

    void resizeImage(QImage *image, const QSize &newSize);

    bool triangulate_step();
};

#endif // TRIANGULED_IMAGE_H
