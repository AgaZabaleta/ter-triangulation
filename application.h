#ifndef APPLICATION_H
#define APPLICATION_H

#include <QtWidgets>

#include "opengl_triangles.h"
#include "trianguled_image.h"
#include "ui_saliencyform.h"
#include "saliencyform.h"
#include "testform.h"

class Trianguled_image;

class Application : public QWidget
{
    Q_OBJECT

public:
    Application(QWidget* parent);

private slots:
    void open();
    void save();
    void switchIm();
    void saliency();
    void step();
    void step10();
    void addAllPoints();
    void test();
    void testSettings();
    void reset();

private:
    bool saveFile(const QByteArray &fileFormat);

    QHBoxLayout *main_layout;
    QVBoxLayout *menu_layout;

    QPushButton *openButton;
    QPushButton *saveButton;
    QPushButton *switchButton;
    QPushButton *saliencyButton;
    QPushButton *stepButton;
    QPushButton *step10Button;
    QPushButton *addAllPointsButton;
    QPushButton *testButton;
    QPushButton *testSettingsButton;
    QPushButton *resetButton;

    SaliencyForm *dialog;
    testForm *test_form;

    Trianguled_image *image_area;
    OpenGLTriangles *opengl_area;
};

#endif
