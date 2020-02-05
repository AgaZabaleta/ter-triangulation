#include <QtWidgets>
#include "trianguled_image.h"

Trianguled_image::Trianguled_image(int n_rows, int n_columns, QWidget *parent):n_y(n_rows), n_x(n_columns), QWidget(parent), points(n_rows * n_columns), tab_triangles()
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

            painter.setPen(QPen(QColor(255,0,0), 10, Qt::SolidLine,  Qt::RoundCap, Qt::RoundJoin));
            painter.drawPoint(*curr_point);
            painter.drawImage(QPoint(0,0), triangles);
        }
        if(tab_triangles.size() > 0) {
            painter.setPen(QPen(QColor(0,0,0), 1, Qt::SolidLine,  Qt::RoundCap, Qt::RoundJoin));
            for(Triangle* curr_triangle : tab_triangles) {
                curr_triangle->draw_triangle(&painter);
            }
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
        int scale_x = image.width() / n_x;
        int scale_y = image.height() / n_y;

        for(int i=0 ; i<n_x+1 ; i++){
            for(int j=0 ; j<n_y+1 ; j++){
                points.push_back(new QPoint(i*scale_x, j*scale_y));
            }
        }
        for(int i = 0; i<2; i++) {
        for(int j = 0; j<2; j++) {
            tab_triangles.push_back(new Triangle(points[j*n_x+i], points[j*n_x+i+1], points[(j+1)*n_x+i]));
        }}
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
