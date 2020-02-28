#include <QtWidgets>
#include "trianguled_image.h"

Trianguled_image::Trianguled_image(int n_rows, int n_columns, QWidget *parent): QWidget(parent), n_y(n_rows), n_x(n_columns), points(n_rows * n_columns), tab_triangles()
{
    setAttribute(Qt::WA_StaticContents);
}

bool Trianguled_image::openImage(const QString &fileName)
{
    QImage loadedImage;
    if (!loadedImage.load(fileName))
        return false;
    backupImage = loadedImage;
//    QSize newSize = size();
//    resizeImage(&loadedImage, newSize);
//    resize(newSize);
    image = loadedImage;

    for(QPointF* curr_point : points) {
        delete(curr_point);
    }
    for(Triangle* curr_triangle : tab_triangles) {
        delete(curr_triangle);
    }
    points.clear();
    tab_triangles.clear();

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
    QImage newImage = image->scaled(newSize.width(), newSize.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QPainter painter(&newImage);
    *image = newImage;
    painter.drawImage(QPoint(0, 0), *image);
}

void Trianguled_image::paintEvent(QPaintEvent *event)
{

    QPainter painter(this);


    if(!image.isNull()){
        QRect dirtyRect = event->rect();

        painter.setRenderHint(QPainter::Antialiasing);

        QSize pixSize = image.size();
        pixSize.scale(event->rect().size(), Qt::KeepAspectRatio);

        QImage scaledImage = image.scaled(pixSize,
                                       Qt::KeepAspectRatio,
                                       Qt::SmoothTransformation
                                       );
        painter.drawImage(dirtyRect, scaledImage, dirtyRect);
//        QSize newSize = event->rect().size();
//        resizeImage(&image, newSize);

//        painter.drawImage(dirtyRect, image, dirtyRect);

        for(QPointF* real_point : points) {
            QPoint curr_point(
                        static_cast<int>(real_point->x()*scaledImage.width()),
                        static_cast<int>(real_point->y()*scaledImage.height())
                        );
            painter.setPen(QPen(QColor(255,0,0), 10, Qt::SolidLine,  Qt::RoundCap, Qt::RoundJoin));
            painter.drawPoint(curr_point);
            painter.drawImage(QPoint(0,0), triangles);
        }
        if(tab_triangles.size() > 0) {
            painter.setPen(QPen(QColor(0,0,0), 1, Qt::SolidLine,  Qt::RoundCap, Qt::RoundJoin));
            for(Triangle* curr_triangle : tab_triangles) {
                QPoint p1(static_cast<int>(points[curr_triangle->getP1()]->x() * scaledImage.width()),
                          static_cast<int>(points[curr_triangle->getP1()]->y() * scaledImage.height())
                        );
                QPoint p2(static_cast<int>(points[curr_triangle->getP2()]->x() * scaledImage.width()),
                          static_cast<int>(points[curr_triangle->getP2()]->y() * scaledImage.height())
                        );
                QPoint p3(static_cast<int>(points[curr_triangle->getP3()]->x() * scaledImage.width()),
                          static_cast<int>(points[curr_triangle->getP3()]->y() * scaledImage.height())
                        );

                painter.drawLine(p1, p2);
                painter.drawLine(p2, p3);
                painter.drawLine(p3, p1);
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

    for(QPointF* real_point : points) {
        QPoint curr_point(static_cast<int>(real_point->x()*image.width()), static_cast<int>(real_point->y()*image.height()));
        best_point = getBestPoint(curr_point);
        if(curr_point.x() != best_point.x() || curr_point.y() != best_point.y()){
            next_point = getNextPoint(curr_point, best_point);
            curr_point.setX(next_point.x());
            curr_point.setY(next_point.y());

            real_point->setX(curr_point.x() / static_cast<double>(image.width()));
            real_point->setY(curr_point.y() / static_cast<double>(image.height()));
        }
    }
    update();
    return false;
}

void Trianguled_image::addPoints(){
    if(!image.isNull()) {
        double scale_x = image.width() / static_cast<double>(n_x-1);
        double scale_y = image.height() / static_cast<double>(n_y-1);

        for(int i=0 ; i<n_x ; i++){
            for(int j=0 ; j<n_y ; j++){
                double pos_x = (i*scale_x) / static_cast<double>(image.width());
                double pos_y = (j*scale_y) / static_cast<double>(image.height());

                points.push_back(new QPointF(pos_x, pos_y));
            }
        }
        for(int i = 0; i<n_x-1; i++) {
            for(int j = 0; j<n_y-1; j++) {
                tab_triangles.push_back(new Triangle(i*n_y+j, i*n_y+j+1, (i+1)*n_y+j));
            }
        }

        for(int i = 1; i<n_x; i++) {
            for(int j = 1; j<n_y; j++) {
                tab_triangles.push_back(new Triangle(i*n_y+j, (i-1)*n_y+j, i*n_y+j-1));
            }
        }

        update();
    } else {
        qInfo() << "No image to add points";
    }
}


void Trianguled_image::addRandomPoint()
{
//    if(!image.isNull()) {
//        qInfo() << "Added point";
//        int x = QRandomGenerator::global()->bounded(0, image.width());
//        int y = QRandomGenerator::global()->bounded(0, image.height());
//        points.push_back(new QPoint(x, y));

//        update();
//    } else {
//        qInfo() << "No image to add point";
//    }
}

std::vector<QPointF*> Trianguled_image::getPoints(){
    return this->points;
}

std::vector<Triangle*> Trianguled_image::getTriangles(){
    return this->tab_triangles;
}
