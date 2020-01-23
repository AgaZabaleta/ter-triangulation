#ifndef APPLICATION_H
#define APPLICATION_H

#include <QtWidgets>

class Trianguled_image;

class Application : public QWidget
{
    Q_OBJECT

public:
    Application(QWidget* parent);

private slots:
    void open();
    void save();
    void grey();
    void step();
    void addPoint();
    void addAllPoints();

private:
    bool saveFile(const QByteArray &fileFormat);

    QHBoxLayout *main_layout;
    QVBoxLayout *menu_layout;

    QPushButton *openButton;
    QPushButton *saveButton;
    QPushButton *greyButton;
    QPushButton *stepButton;
    QPushButton *addPointButton;
    QPushButton *addAllPointsButton;


    Trianguled_image *image_area;
};

#endif
