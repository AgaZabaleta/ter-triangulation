#include <algorithm>
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

void Trianguled_image::saliency(double gradient_value = 0.0,double  color_value = 0.0,double  texture_value = 0.0){
    if(!image.isNull()) {
        if(!image.allGray()){
            backupImage = image;
            image = image.convertToFormat(QImage::Format_Grayscale8);
        }
        QImage gradient_image = gradient_saliency();
        QImage color_image = color_saliency();
        QImage texture_image = texture_saliency();

        double test_total = gradient_value + color_value + texture_value;
        if(test_total != 1.0){
            gradient_value = gradient_value / test_total;
            color_value = color_value / test_total;
            texture_value = texture_value / test_total;
        }

        QImage res = QImage(image);
        for(int i=0 ; i<res.width() ; i++){
            for(int j=0 ; j<res.height() ; j++){
                double d_value = gradient_image.pixelColor(i,j).blue() * gradient_value
                        + color_image.pixelColor(i,j).blue() * color_value
                        + texture_image.pixelColor(i,j).blue() * texture_value;

                QRgb rgb_value = qRgb(static_cast<int>(d_value), static_cast<int>(d_value), static_cast<int>(d_value));
                res.setPixel(i, j, rgb_value);
            }
        }
        image = res;
        update();
        qInfo() << "Image transformed";
    } else {
        qInfo() << "No image to transform";
    }
}

QImage Trianguled_image::gradient_saliency(){
    if(!image.isNull()) {
        QImage res = QImage(image);
        //G=sqrt(p(i,j)-p(i-1,j)2) +
        for(int i=0 ; i<image.width() ; i++){
            for(int j=0 ; j<image.height() ; j++){
                double g1 = 0;
                double g2 = 0;
                double g3 = 0;
                double g4 = 0;
                if(i>0){
                    g1 = pow(image.pixelColor(i, j).blue() - image.pixelColor(i-1, j).blue(),2);
                }
                if(i<image.width()-1){
                    g2 = pow(image.pixelColor(i, j).blue() - image.pixelColor(i+1, j).blue(),2);
                }
                if(j>0){
                    g3 = pow(image.pixelColor(i, j).blue() - image.pixelColor(i, j-1).blue(),2);
                }
                if(j<image.height()-1){
                    g4 = pow(image.pixelColor(i, j).blue() - image.pixelColor(i, j+1).blue(),2);
                }
                double G = sqrt(g1 + g2 + g3 + g4);

                QRgb value = qRgb(static_cast<int>(G), static_cast<int>(G), static_cast<int>(G));
                res.setPixel(i , j, value);
            }
        }
        qInfo() << "Gradient saliency done";
        return res;
    }
    return QImage(image);
}

QImage Trianguled_image::color_saliency(){
    if(!image.isNull()) {
        QImage res = QImage(image.width(), image.height(), image.format());
        for(int i=0 ; i<image.width() ; i++){
            for(int j=0 ; j<image.height() ; j++){
                double g1[3] = {0,0,0};
                double g2[3] = {0,0,0};
                double g3[3] = {0,0,0};
                double g4[3] = {0,0,0};
                if(i>0){
                    g1[0] = pow(backupImage.pixelColor(i, j).red() - backupImage.pixelColor(i-1, j).red(),2);
                    g1[1] = pow(backupImage.pixelColor(i, j).green() - backupImage.pixelColor(i-1, j).green(),2);
                    g1[2] = pow(backupImage.pixelColor(i, j).blue() - backupImage.pixelColor(i-1, j).blue(),2);
                }
                if(i<image.width()-1){
                    g2[0] = pow(backupImage.pixelColor(i, j).red() - backupImage.pixelColor(i+1, j).red(),2);
                    g2[1] = pow(backupImage.pixelColor(i, j).green() - backupImage.pixelColor(i+1, j).green(),2);
                    g2[2] = pow(backupImage.pixelColor(i, j).blue() - backupImage.pixelColor(i+1, j).blue(),2);
                }
                if(j>0){
                    g3[0] = pow(backupImage.pixelColor(i, j).red() - backupImage.pixelColor(i, j-1).red(),2);
                    g3[1] = pow(backupImage.pixelColor(i, j).green() - backupImage.pixelColor(i, j-1).green(),2);
                    g3[2] = pow(backupImage.pixelColor(i, j).blue() - backupImage.pixelColor(i, j-1).blue(),2);
                }
                if(j<image.height()-1){
                    g4[0] = pow(backupImage.pixelColor(i, j).red() - backupImage.pixelColor(i, j+1).red(),2);
                    g4[1] = pow(backupImage.pixelColor(i, j).green() - backupImage.pixelColor(i, j+1).green(),2);
                    g4[2] = pow(backupImage.pixelColor(i, j).blue() - backupImage.pixelColor(i, j+1).blue(),2);
                }
                double G[3];
                G[0] = sqrt(g1[0] + g2[0] + g3[0] + g4[0]);
                G[1] = sqrt(g1[1] + g2[1] + g3[1] + g4[1]);
                G[2] = sqrt(g1[2] + g2[2] + g3[2] + g4[2]);

                double Gres = 0;
                for(int i=0 ; i<3 ; i++){
                    if(G[i] > Gres){
                        Gres = G[i];
                    }
                }


                QRgb value = qRgb(static_cast<int>(Gres), static_cast<int>(Gres), static_cast<int>(Gres));
                res.setPixel(i , j, value);
            }
        }

        qInfo() << "Color saliency done";
        return res;
    }
    return QImage(image);
}

QImage Trianguled_image::texture_saliency(){
    return QImage(image);
}


void Trianguled_image::triangulate()
{
    do {
        point_moved = triangulate_step();
    } while(point_moved);

    //qInfo() << "finish tri";
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
                        if(point_tmp_value > point_res_value){
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
        if((real_point->x() > 0 && real_point->x() < 1) && (real_point->y() > 0 && real_point->y() < 1)) {
            QPoint curr_point(static_cast<int>(real_point->x()*(image.width()-1)), static_cast<int>(real_point->y()*(image.height()-1)));
            best_point = getBestPoint(curr_point);

            if(curr_point.x() != best_point.x() || curr_point.y() != best_point.y()){
                next_point = getNextPoint(curr_point, best_point);
                curr_point.setX(next_point.x());
                curr_point.setY(next_point.y());

                real_point->setX(curr_point.x() / static_cast<double>(image.width()));
                real_point->setY(curr_point.y() / static_cast<double>(image.height()));
            }
        }
    }
    update();
    return false;
}

void Trianguled_image::addPoints(){
    if(!image.isNull()) {
        double scale_x = getScaleX();
        double scale_y = getScaleY();

        for(int i=0 ; i<n_x ; i++){
            for(int j=0 ; j<n_y ; j++){
                double pos_x = (i*scale_x) / static_cast<double>(image.width());
                double pos_y = (j*scale_y) / static_cast<double>(image.height());

                //qInfo() << pos_x << " " << pos_y;
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

QImage Trianguled_image::getImage(){
    return this->image;
}

std::vector<QPointF*> Trianguled_image::getPoints(){
    return this->points;
}

std::vector<Triangle*> Trianguled_image::getTriangles(){
    return this->tab_triangles;
}

double Trianguled_image::getScaleX() {
    return image.width() / static_cast<double>(n_x-1);
}

double Trianguled_image::getScaleY() {
    return image.height() / static_cast<double>(n_y-1);
}

void Trianguled_image::setN_xy(double percent) {
    n_x = static_cast<int>(round(1.0 / percent));
    n_y = static_cast<int>(n_x * (image.height() / static_cast<double>(image.width())));
}

QColor Trianguled_image::getPointColor(int i) {
    QPoint curr_point(static_cast<int>((points[i])->x()*(image.width()-1)), static_cast<int>((points[i])->y()*(image.height()-1)));
    return image.pixelColor(curr_point);
}

bool within(double x)
{
    return 0 <= x && x <= 1;
}

bool in_triangle(QPoint p, QPoint a, QPoint b, QPoint c) {
    double det = (b.y() - c.y())*(a.x() - c.x()) + (c.x() - b.x())*(a.y() - c.y());
    double factor_alpha = (b.y() - c.y())*(p.x() - c.x()) + (c.x() - b.x())*(p.y() - c.y());
    double factor_beta = (c.y() - a.y())*(p.x() - c.x()) + (a.x() - c.x())*(p.y() - c.y());
    double alpha =  factor_alpha / det;
    double beta =  factor_beta / det;
    double gamma = 1.0 - alpha - beta;

    return p == a || p == b || p == c || (within(alpha) && within(beta) && within(gamma));
}

QColor Trianguled_image::getTriangleColor(int p1, int p2, int p3) {
    QColor result(0,0,0);

    QPoint a(static_cast<int>((points[p1])->x()*(image.width()-1)), static_cast<int>((points[p1])->y()*(image.height()-1)));
    QPoint b(static_cast<int>((points[p2])->x()*(image.width()-1)), static_cast<int>((points[p2])->y()*(image.height()-1)));
    QPoint c(static_cast<int>((points[p3])->x()*(image.width()-1)), static_cast<int>((points[p3])->y()*(image.height()-1)));

    QPoint p;
    QColor p_color;
    double red = 0;
    double green = 0;
    double blue = 0;
    int n_pixel = 0;

    int min_x = std::min(a.x(), std::min(b.x(), c.x()));
    int min_y = std::min(a.y(), std::min(b.y(), c.y()));
    int max_x = std::max(a.x(), std::max(b.x(), c.x()));
    int max_y = std::max(a.y(), std::max(b.y(), c.y()));

    for(int x = min_x; x <= max_x; x++) {
        for(int y = min_y; y <= max_y; y++) {
            p = QPoint(x, y);
            if (in_triangle(p, a, b, c)) {
                n_pixel++;
                p_color = backupImage.pixelColor(p);
                red += p_color.redF();
                blue += p_color.blueF();
                green += p_color.greenF();
            }
        }
    }

    if(n_pixel > 0) {
        result.setRedF(red / n_pixel);
        result.setBlueF(blue / n_pixel);
        result.setGreenF(green / n_pixel);
    }

    return result;
}

