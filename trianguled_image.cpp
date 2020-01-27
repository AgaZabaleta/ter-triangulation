#include <QtWidgets>
#include "trianguled_image.h"

Trianguled_image::Trianguled_image(QWidget *parent):QWidget(parent), points()
{
    setAttribute(Qt::WA_StaticContents);


}

bool Trianguled_image::openImage(const QString &fileName)
{
    QImage loadedImage;
    if (!loadedImage.load(fileName))
        return false;
    QSize newSize = size();
    resizeImage(&loadedImage, newSize);
    resize(newSize);
    image = loadedImage;
    backupImage = image;
    for(QPoint* curr_point : points) {
        delete(curr_point);
    }
    points.clear();

    update();
    return true;
}

bool Trianguled_image::saveImage(const QString &fileName, const char *fileFormat)
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
{
    if (image->size() == newSize)
        return;
    QImage newImage = image->scaled(newSize.width(), newSize.height());
    QPainter painter(&newImage);
    *image = newImage;
    painter.drawImage(QPoint(0, 0), *image);
}

void Trianguled_image::paintEvent(QPaintEvent *event)
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

void Trianguled_image::transformToGrey(){
    if(!image.isNull()) {
        if(image.allGray()){
            image = backupImage;
        }else{
            backupImage = image;
            image = image.convertToFormat(QImage::Format_Grayscale8);
        }
        update();
        qInfo() << "Image transformed";
    } else {
        qInfo() << "No image to transform";
    }
}

void Trianguled_image::triangulate()
{
    do {
        point_moved = triangulate_step();
    } while(point_moved);

    qInfo() << "finish tri";
}

int Trianguled_image::getPointValue(QPoint point){
    return image.pixelColor(point).red();
}


QPoint Trianguled_image::getBestPoint(QPoint point){
    int px = point.x();
    int py = point.y();
    double distToPoint;
    QPoint point_res = point;
    int point_res_value = getPointValue(point);
    double point_res_dist = 0;
    QPoint point_tmp;
    int point_tmp_value;

    for(int i=(-1)*vision_range+px ; i<vision_range+px ; i++){
        if(i>=0 && i<image.width()){
            for(int j=(-1)*vision_range+py ; j<vision_range+py ; j++){
                if(j>=0 && j<image.height()){
                    distToPoint = sqrt(pow(i-px, 2)+pow(j-py, 2));
                    if(distToPoint<=vision_range){
                        point_tmp = QPoint(i,j);
                        point_tmp_value = getPointValue(point_tmp);
                        if(point_tmp_value < point_res_value){
                            point_res = point_tmp;
                            point_res_value = point_tmp_value;
                            point_res_dist = distToPoint;
                        }else if(point_tmp_value == point_res_value && distToPoint<point_res_dist){
                            point_res = point_tmp;
                            point_res_value = point_tmp_value;
                            point_res_dist = distToPoint;
                        }
                    }
                }
            }
        }
    }
    update();
    return point_res;
}

QPoint Trianguled_image::getNextPoint(QPoint p_origin, QPoint p_best){
    QPoint p_next;
    int x_move = 0;
    int y_move = 0;
    if(p_origin.x() != p_best.x()){
        x_move = (p_best.x() - p_origin.x()) / abs(p_best.x() - p_origin.x());
    }
    if(p_origin.y() != p_best.y()){
        y_move = (p_best.y() - p_origin.y()) / abs(p_best.y() - p_origin.y());
    }
    p_next.setX(p_origin.x() + x_move);
    p_next.setY(p_origin.y() + y_move);
    return p_next;
}


bool Trianguled_image::triangulate_step()
{
    QPoint best_point;
    QPoint next_point;
    for(QPoint* curr_point : points) {
        best_point = getBestPoint(*curr_point);
        if(curr_point->x() != best_point.x() || curr_point->y() != best_point.y()){
            next_point = getNextPoint(*curr_point, best_point);
            curr_point->setX(next_point.x());
            curr_point->setY(next_point.y());
        }
    }
    update();
    return false;
}

void Trianguled_image::addPoints(){
    if(!image.isNull()) {
        int scale_x = image.width() / 10;
        int scale_y = image.height() / 10;
        for(int i=0 ; i<image.width() ; i+=scale_x){
            for(int j=0 ; j<image.height() ; j+=scale_y){
                points.push_back(new QPoint(i, j));
            }
        }
        update();
    } else {
        qInfo() << "No image to add points";
    }
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
