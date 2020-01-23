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
    void step();
    void addPoint();

private:
    bool saveFile(const QByteArray &fileFormat);

    QHBoxLayout *main_layout;
    QVBoxLayout *menu_layout;

    QPushButton *openButton;
    QPushButton *saveButton;
    QPushButton *stepButton;
    QPushButton *addPointButton;

    Trianguled_image *image_area;
};

#endif
