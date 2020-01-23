#include <QtWidgets>
#include "trianguled_image.h"

Trianguled_image::Trianguled_image(QWidget *parent):QWidget(parent), points()
{
    setAttribute(Qt::WA_StaticContents);


}

bool Trianguled_image::openImage(const QString &fileName)
//! [1] //! [2]
{
    QImage loadedImage;
    if (!loadedImage.load(fileName))
        return false;

    QSize newSize = loadedImage.size().expandedTo(size());
    resizeImage(&loadedImage, newSize);
    resize(newSize);
    image = loadedImage;

    for(QPoint* curr_point : points) {
        delete(curr_point);
    }
    points.clear();

    update();
    return true;
}

bool Trianguled_image::saveImage(const QString &fileName, const char *fileFormat)
//! [3] //! [4]
{
    QImage visibleImage = image;
    resizeImage(&visibleImage, size());

    if (visibleImage.save(fileName, fileFormat)) {
        return true;
    } else {
        return false;
    }
}

void Trianguled_image::resizeImage(QImage *image, const QSize &newSize)
//! [19] //! [20]
{
    if (image->size() == newSize)
        return;

    QImage newImage(newSize, QImage::Format_RGB32);
    newImage.fill(qRgb(255, 255, 255));
    QPainter painter(&newImage);
    painter.drawImage(QPoint(0, 0), *image);
    *image = newImage;
}

void Trianguled_image::paintEvent(QPaintEvent *event)
//! [13] //! [14]
{

    QPainter painter(this);
    QRect dirtyRect = event->rect();
    painter.drawImage(dirtyRect, image, dirtyRect);

    if(!image.isNull()){
        for(QPoint* curr_point : points) {
            qInfo() << "Point drawn : " << curr_point->x() << " " << curr_point->y();
            painter.setPen(QPen(QColor(255,0,0), 10, Qt::SolidLine,  Qt::RoundCap, Qt::RoundJoin));
            painter.drawPoint(*curr_point);
            painter.drawImage(QPoint(0,0), triangles);
        }
    }
}

void Trianguled_image::triangulate()
{
    do {
        point_moved = triangulate_step();
    } while(point_moved);

    qInfo() << "finish tri";
}

bool Trianguled_image::triangulate_step()
{
    for(QPoint* curr_point : points) {
        int dx = QRandomGenerator::global()->bounded(-2, 2);
        int dy = QRandomGenerator::global()->bounded(-2, 2);
        curr_point->setX(curr_point->x() + dx);
        curr_point->setY(curr_point->y() + dy);
    }
    update();
    return false;
}
void Trianguled_image::addRandomPoint()
{
    if(!image.isNull()) {
        qInfo() << "Added point";
        int x = QRandomGenerator::global()->bounded(0, image.width());
        int y = QRandomGenerator::global()->bounded(0, image.height());
        points.push_back(new QPoint(x, y));

        update();
    } else {
        qInfo() << "No image to add point";
    }
}
