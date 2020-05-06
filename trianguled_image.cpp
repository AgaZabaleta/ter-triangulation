#include <algorithm>
#include <QtWidgets>
#include "trianguled_image.h"

Trianguled_image::Trianguled_image(int n_rows, int n_columns, QWidget *parent): QWidget(parent), n_y(n_rows), n_x(n_columns), points(n_rows * n_columns), tab_triangles(), neighbours(), adjacent_triangles()
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

    neighbours.clear();
    adjacent_triangles.clear();

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
            painter.setPen(QPen(QColor(0,0,0), 5, Qt::SolidLine,  Qt::RoundCap, Qt::RoundJoin));
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

void Trianguled_image::switchImage(){
    if(!image.isNull() && !saliencyMap.isNull()) {
        if(image == backupImage){
//            image = saliencyMap;
            for(int i=0 ; i<saliencyMap.width() ; i++){
                for(int j=0 ; j<saliencyMap.height() ; j++){
                    QColor color_value = saliencyMap.pixelColor(i,j);
                    color_value.setHsvF(0.98 * color_value.redF(), 0.8,0.8);
                    image.setPixelColor(i, j, color_value);
                }
            }
        }else{
            image = backupImage;
        }
        update();
        qInfo() << "Image transformed";
    } else {
        qInfo() << "No image to switch";
    }
}

void Trianguled_image::saliency(double gradient_value = 0.0,double  color_value = 0.0,double  texture_value = 0.0, bool recurrent = false){
    if(!image.isNull()) {
        if(!image.allGray()){
            backupImage = image;
            image = image.convertToFormat(QImage::Format_Grayscale8);
        }

        QImage gradient_image;
        QImage color_image;
        QImage texture_image;
        if(gradient_value == 0.0){
            gradient_image = QImage(image);
        }else{
            gradient_image = gradient_saliency();
        }
        if(color_value == 0.0){
            color_image = QImage(image);
        }else{
            color_image = color_saliency();
        }
        if(texture_value == 0.0){
            texture_image = QImage(image);
        }else{
            texture_image = texture_saliency(recurrent);
        }

        double test_total = gradient_value + color_value + texture_value;
        if(test_total != 1.0){
            gradient_value = gradient_value / test_total;
            color_value = color_value / test_total;
            texture_value = texture_value / test_total;
        }

        QImage res = QImage(image);
        image = image.convertToFormat(QImage::Format_RGBA64);
        for(int i=0 ; i<res.width() ; i++){
            for(int j=0 ; j<res.height() ; j++){
                double d_value = gradient_image.pixelColor(i,j).blue() * gradient_value
                        + color_image.pixelColor(i,j).blue() * color_value
                        + texture_image.pixelColor(i,j).blue() * texture_value;

                QRgb rgb_value = qRgb(static_cast<int>(d_value), static_cast<int>(d_value), static_cast<int>(d_value));
                res.setPixelColor(i, j, rgb_value);

                QColor color_value = QColor();
                color_value.setHsvF(0.98 * (d_value/255), 0.8,0.8);
                image.setPixelColor(i, j, color_value);
            }
        }
        saliencyMap = res;
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
        for(int i=0 ; i<backupImage.width() ; i++){
            for(int j=0 ; j<backupImage.height() ; j++){
                qreal h, s, v;
                backupImage.pixelColor(i, j).getHsvF(&h, &s, &v);
                h*=255;
                s*=255;
                v*=255;
                int value = static_cast<int>(sqrt(pow(h,2)+pow(s,2)+pow(v,2)));
                if(value > 255){
                    value = 255;
                }

                QRgb color = qRgb(static_cast<int>(value), static_cast<int>(value), static_cast<int>(value));
                res.setPixel(i, j, color);
            }
        }

        qInfo() << "Color saliency done";
        return res;
    }
    return QImage(image);
}

QImage Trianguled_image::texture_saliency(bool recurrent){
    if(!image.isNull()) {
        QImage res = QImage(image.width(), image.height(), image.format());

        int histo[255];
        for(int i=0 ; i<256 ; i++){
            histo[i] = 0;
        }
        for(int i=0 ; i<image.width() ; i++){
            for(int j=0 ; j<image.height() ; j++){
                if(i==0 || i==image.width()-1 || j==0 || j==image.width()-1){
                    res.setPixel(i,j,0);
                }else{
                    int expo = 1;
                    int value = 0;
                    for(int y_delta=-1 ; y_delta<=1 ; y_delta++){
                        for(int x_delta=-1 ; x_delta<=1 ; x_delta++){
                            if(x_delta != 0 || y_delta != 0){
                                int diff = image.pixelColor(i+x_delta, j+y_delta).blue() - image.pixelColor(i, j).blue();
                                if(diff>=0){
                                    diff = 1;
                                }else{
                                    diff = 0;
                                }
                                value+=diff*expo;
                                expo*=2;
                            }
                        }
                    }
                    if(recurrent){
                        histo[value]++;
                    }
                    QRgb rgb_value = qRgb(value, value, value);
                    res.setPixel(i,j,rgb_value);
                }
            }
        }

        int new_histo[255];

        for(int i=0 ; i<256 ; i++){
            new_histo[i] = static_cast<int>(histo[i]/static_cast<double>(image.width()*image.height()) * 255);
        }
        int max = 0;
        for(int i=0 ; i<256 ; i++){
            if(max < new_histo[i]){
                max = new_histo[i];
            }
        }
        if(recurrent){
            for(int i=0 ; i<image.width() ; i++){
                for(int j=0 ; j<image.height() ; j++){
                    if(!(i==0 || i==image.width()-1 || j==0 || j==image.width()-1)){
                        int new_value = static_cast<int>(new_histo[res.pixelColor(i,j).blue()] / static_cast<double>(max) * 255.0);
                        QRgb new_value_rgb = qRgb(new_value, new_value, new_value);
                        res.setPixel(i,j,new_value_rgb);
                    }
                }
            }
        }

        qInfo() << "Texture saliency done";
        return res;
    }
    return QImage(image);
}


void Trianguled_image::triangulate()
{
    do {
        point_moved = triangulate_step();
    } while(point_moved);

}

int Trianguled_image::getPointValue(QPoint point){
    return saliencyMap.pixelColor(point).red();
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
        if(i>=0 && i<saliencyMap.width()){
            for(int j=(-1)*vision_range+py ; j<vision_range+py ; j++){
                if(j>=0 && j<saliencyMap.height()){
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
    if(saliencyMap.isNull()){
        qInfo() << "No saliency map";
        return false;
    }

    points_buffer.clear();
    for(int i = 0; i < static_cast<int>(points.size()); i++) {
//        qInfo() << getPointError(i, points);
        points_buffer.push_back(new QPointF(*points[i]));
    }

    step_saliency(points_buffer, points_buffer);
    step_error(points_buffer, points_buffer);
    step_laplacian(points_buffer, points, laplacian_value_test);

    update();
    return false;
}

void Trianguled_image::step_saliency(std::vector<QPointF*> &source_points, std::vector<QPointF*> &dest_points)
{
//    dest_points.clear();
//    for(int i=0 ; i<static_cast<int>(points.size()) ; i++){
//        points_step.push_back(new QPointF(*points[i]));
//    }

    QPoint best_point;
    QPoint next_point;

    for(int i = 0; i < static_cast<int>(source_points.size()); i++) {
        QPointF* real_point = source_points[i];

        if((real_point->x() > 0 && real_point->x() < 1) && (real_point->y() > 0 && real_point->y() < 1)) {
            QPoint curr_point(static_cast<int>(real_point->x()*(saliencyMap.width()-1)), static_cast<int>(real_point->y()*(saliencyMap.height()-1)));
            best_point = getBestPoint(curr_point);

            if(curr_point.x() != best_point.x() || curr_point.y() != best_point.y()){
                next_point = getNextPoint(curr_point, best_point);
                curr_point.setX(next_point.x());
                curr_point.setY(next_point.y());

//                real_point->setX(curr_point.x() / static_cast<double>(saliencyMap.width()));
//                real_point->setY(curr_point.y() / static_cast<double>(saliencyMap.height()));

//                QPointF* new_point = new QPointF();
//                new_point->setX(curr_point.x() / static_cast<double>(saliencyMap.width()));
//                new_point->setY(curr_point.y() / static_cast<double>(saliencyMap.height()));

//                dest_points.push_back(new_point);

                dest_points[i]->setX(curr_point.x() / static_cast<double>(saliencyMap.width()));
                dest_points[i]->setY(curr_point.y() / static_cast<double>(saliencyMap.height()));

            }
        }
    }
}

void Trianguled_image::step_error(std::vector<QPointF*> &source_points, std::vector<QPointF*> &dest_points)
{
    for(int i = 0; i < static_cast<int>(source_points.size()); i++) {
        QPointF* real_point = source_points[i];
        if((real_point->x() > 0 && real_point->x() < 1) && (real_point->y() > 0 && real_point->y() < 1)) { // On prends pas les points au bord
            // On initialise min_error à l'erreur d'origine
            // et min_point au point de base
            double min_error = getPointError(i, source_points);
            QPointF* min_point = new QPointF(*real_point);
            for(int k=0 ; k<4 ; k++){ // k=1 : on regarde que le point a droite ; k=4 : on regarde haut bas gauche droite ; k=8 on regarde les diag. en plus
                for(int j=1 ; j<=cardinal_range ; j++){ // La nombre de point qu'on va regarder dans une direction (cardinal_range se trouve dans le .h)
                    // transformation en qpoint pour decaler les px
                    QPoint transi(static_cast<int>(real_point->x()*(image.width()-1)), static_cast<int>(real_point->y()*(image.height()-1)));
                    QPoint old = QPoint(transi);
                    // On decale dans les sens souhaités
                    if(k==0){
                        transi.setX(transi.x()+j);
                    }else if(k==1){
                        transi.setY(transi.y()+j);
                    }else if(k==2){
                        transi.setX(transi.x()-j);
                    }else if(k==3){
                        transi.setY(transi.y()-j);
                    }else if(k==4){
                        transi.setX(transi.x()+j);
                        transi.setY(transi.y()+j);
                    }else if(k==5){
                        transi.setX(transi.x()-j);
                        transi.setY(transi.y()-j);
                    }else if(k==6){
                        transi.setX(transi.x()+j);
                        transi.setY(transi.y()-j);
                    }else if(k==7){
                        transi.setX(transi.x()-j);
                        transi.setY(transi.y()+j);
                    }
                    // On repasse le qpoint en qpointF
                    QPointF* test_error = new QPointF(transi.x()/static_cast<double>(image.width()), transi.y()/static_cast<double>(image.height()));
                    if((test_error->x() > 0 && test_error->x() < 1) && (test_error->y() > 0 && test_error->y() < 1)) {
                        // On le met dans le tableau points_step
                        source_points[i] = test_error;
                        // On calcule l'erreur du nouveau point
                        double test_error_value = getPointError(i, source_points);
                        // Si l'erreur est minimisée par ce point
    //                    if(i == source_points.size() / 2 + 13 && k == 1) {
    //                        qInfo() << "previous error: " << min_error << "; curr_error: " << test_error_value;
    //                        qInfo() << "for: " << *real_point << " " << *min_point << " " << *test_error << " and " << *source_points[i];
    //                    }

                        if(test_error_value < min_error){
    //                        qInfo() << "less! at " << i;
                            // On remplace min_error / min_point
                            min_error = test_error_value;
                            min_point = test_error;
                        }
                        // On remet le point d'origine pour refaire les tests
                        source_points[i] = real_point;
                    }
                }
            }

            // On change par le point qui minimise l'erreur
            dest_points[i] = min_point;
        }
    }
}

void Trianguled_image::step_laplacian(std::vector<QPointF*> &source_points, std::vector<QPointF*> &dest_points, float weight)
{
    for(int i = 0; i < static_cast<int>(source_points.size()); i++) {
        QPointF* real_point = dest_points[i];
        if((real_point->x() > 0 && real_point->x() < 1) && (real_point->y() > 0 && real_point->y() < 1)) {
            QPointF ring_center = getBarycenter(i, source_points);
            QPointF curr_point = *source_points[i];
            QPointF distance = (ring_center - curr_point);
            double new_x, new_y;
            new_x = curr_point.x() + weight * distance.x();
            new_y = curr_point.y() + weight * distance.y();

            real_point->setX(new_x);
            real_point->setY(new_y);
        }
    }
}

void Trianguled_image::addNeighbor(int i, int j){
    bool exists = false;
    for(int k=0 ; k<neighbours[i].size() ; k++){
        if(neighbours[i][k] == j){
            exists = true;
            break;
        }
    }
    if(!exists){
        neighbours[i].push_back(j);
    }

}

void Trianguled_image::addAdjacentTriangle(int i, int triangle){
    bool exists = false;
    for(int k=0 ; k<adjacent_triangles[i].size() ; k++){
        if(adjacent_triangles[i][k] == triangle){
            exists = true;
            break;
        }
    }
    if(!exists){
        adjacent_triangles[i].push_back(triangle);
    }

}

QPointF Trianguled_image::getBarycenter(int i, std::vector<QPointF*> &source_points){
    double denom = 0.0;
    double nume_x = 0;
    double nume_y = 0;
    for(int j=0 ; j<static_cast<int>(neighbours[i].size()) ; j++){
        nume_x += source_points[neighbours[i][j]]->x();
        nume_y += source_points[neighbours[i][j]]->y();
        denom++;
    }
    double x = nume_x/denom;
    double y = nume_y/denom;
    return QPointF(x,y);
}


void Trianguled_image::addPoints(){
    if(!image.isNull()) {
        double scale_x = getScaleX();
        double scale_y = getScaleY();

        for(int i=0 ; i<n_x ; i++){
            for(int j=0 ; j<n_y ; j++){
                double pos_x = (i*scale_x) / static_cast<double>(image.width());
                double pos_y = (j*scale_y) / static_cast<double>(image.height());

                points.push_back(new QPointF(pos_x, pos_y));
                neighbours.push_back(std::vector<int>());
                adjacent_triangles.push_back(std::vector<int>());
            }
        }
        for(int i = 0; i<n_x-1; i++) {
            for(int j = 0; j<n_y-1; j++) {
                tab_triangles.push_back(new Triangle(i*n_y+j, i*n_y+j+1, (i+1)*n_y+j));
                addNeighbor(i*n_y+j, i*n_y+j+1);
                addNeighbor(i*n_y+j, (i+1)*n_y+j);
                addNeighbor(i*n_y+j+1, i*n_y+j);
                addNeighbor(i*n_y+j+1, (i+1)*n_y+j);
                addNeighbor((i+1)*n_y+j, i*n_y+j);
                addNeighbor((i+1)*n_y+j, i*n_y+j+1);

                int triangle_id = tab_triangles.size() - 1;
//                qInfo() << triangle_id;
                addAdjacentTriangle(i*n_y+j, triangle_id);
                addAdjacentTriangle(i*n_y+j+1, triangle_id);
                addAdjacentTriangle((i+1)*n_y+j, triangle_id);
            }
        }

        for(int i = 1; i<n_x; i++) {
            for(int j = 1; j<n_y; j++) {
                tab_triangles.push_back(new Triangle(i*n_y+j, (i-1)*n_y+j, i*n_y+j-1));
                addNeighbor(i*n_y+j, (i-1)*n_y+j);
                addNeighbor(i*n_y+j, i*n_y+j-1);
                addNeighbor((i-1)*n_y+j, i*n_y+j);
                addNeighbor((i-1)*n_y+j, i*n_y+j-1);
                addNeighbor(i*n_y+j-1, i*n_y+j);
                addNeighbor(i*n_y+j-1, (i-1)*n_y+j);

                int triangle_id = adjacent_triangles.size() - 1;
                addAdjacentTriangle(i*n_y+j, triangle_id);
                addAdjacentTriangle((i-1)*n_y+j, triangle_id);
                addAdjacentTriangle((i*n_y+j-1), triangle_id);
            }
        }
        update();
    } else {
        qInfo() << "No image to add points";
    }
}

void Trianguled_image::test(){
    if(image.isNull()){
        openImage(testImage);
    }
    setN_xy(percent_test);
    saliency(gradient_value_test, color_value_test, texture_value_test, recurrent_value_test);
    addPoints();
}

void Trianguled_image::testSettings(double p, double gv, double cv, double tv, bool rec, double lp){
    percent_test = p;
    gradient_value_test = gv;
    color_value_test = cv;
    texture_value_test = tv;
    recurrent_value_test = rec;
    laplacian_value_test = lp;
}

void Trianguled_image::reset(){
    image = backupImage;
    saliencyMap = QImage();
    triangles = QImage();
    points.clear();
    points_buffer.clear();
    points_final.clear();
    tab_triangles.clear();
    neighbours.clear();
    adjacent_triangles.clear();

    setN_xy();
    update();
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
    setVision_range();
}

void Trianguled_image::setVision_range(){
    vision_range = static_cast<int>(0.5 * image.width() / static_cast<double>(n_x));
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

double Trianguled_image::getTriangleError(int p1, int p2, int p3, std::vector<QPointF*> &source_points) {
    QColor result(0,0,0);
    double error;

    QPoint a, b, c;
    a.setX(static_cast<int>((source_points[p1])->x()*(image.width()-1)));
    a.setY(static_cast<int>((source_points[p1])->y()*(image.height()-1)));
    b.setX(static_cast<int>((source_points[p2])->x()*(image.width()-1)));
    b.setY(static_cast<int>((source_points[p2])->y()*(image.height()-1)));
    c.setX(static_cast<int>((source_points[p3])->x()*(image.width()-1)));
    c.setY(static_cast<int>((source_points[p3])->y()*(image.height()-1)));

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

    double sum_dist = 0;
    double true_color_value;
    double mean_color_value = (result.redF() + result.blueF() + result.greenF()) / 3.0;

    for(int x = min_x; x <= max_x; x++) {
        for(int y = min_y; y <= max_y; y++) {
            p = QPoint(x, y);
            if (in_triangle(p, a, b, c)) {
                p_color = backupImage.pixelColor(p);
                true_color_value = (p_color.redF() + p_color.blueF() + p_color.greenF()) / 3.0;
                sum_dist += (true_color_value - mean_color_value) * (true_color_value - mean_color_value);
            }
        }
    }

    error = sum_dist / static_cast<double>(n_pixel);

    return error;
}

double Trianguled_image::laplacian_dotproduct(int i, std::vector<QPointF*> &source_points){
    double sum = 0;
    QPointF* curr_point = source_points[i];
    int n_neighbours = static_cast<int>(neighbours[i].size());
    for(int j=0 ; j < n_neighbours ; j++){
        QPointF* curr_neighbour = source_points[neighbours[i][j]];
        QPointF distance = *curr_neighbour - *curr_point;
        sum += QPointF::dotProduct(distance, distance);
    }

    return (sum / 2.0 * static_cast<double>(n_neighbours));
}

double Trianguled_image::getPointError(int curr_point, std::vector<QPointF*> &source_points) {
    Triangle* curr_t;
    double curr_err;
    double sum_error = 0;
    for (int adj_triangle_id : adjacent_triangles[curr_point]) {
        curr_t = tab_triangles[adj_triangle_id];
        curr_err = getTriangleError(curr_t->getP1(), curr_t->getP2(), curr_t->getP3(), source_points) / 3.0;
        curr_err += laplacian_value_test * laplacian_dotproduct(curr_point, source_points);
        sum_error += curr_err;
    }

    return sum_error;
}



