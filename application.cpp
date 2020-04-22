#include "application.h"

//! [0]
Application::Application(QWidget* parent) :
    QWidget(parent),
    image_area(new Trianguled_image(15, 15, this)),
    opengl_area(new OpenGLTriangles(this))
{
    opengl_area->linkTrianguledImage(image_area);

    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    main_layout = new QHBoxLayout();
    menu_layout = new QVBoxLayout();

    openButton = new QPushButton("Open", this);
    saveButton = new QPushButton("Save", this);
    switchButton = new QPushButton("Saliency map", this);
    saliencyButton = new QPushButton("Saliency settings", this);
    stepButton = new QPushButton("Step", this);
    step10Button = new QPushButton("Step x10", this);
    addAllPointsButton = new QPushButton("Add all points", this);
    testButton = new QPushButton("Test", this);
    testSettingsButton = new QPushButton("Test settings", this);
    resetButton = new QPushButton("Reset", this);

    openButton->setGeometry(QRect(QPoint(100,100), QSize(200,50)));
    saveButton->setGeometry(QRect(QPoint(100,175), QSize(200,50)));
    switchButton->setGeometry(QRect(QPoint(100,175), QSize(200,50)));
    saliencyButton->setGeometry(QRect(QPoint(100,175), QSize(200,50)));
    stepButton->setGeometry(QRect(QPoint(100,250), QSize(200,50)));
    step10Button->setGeometry(QRect(QPoint(100,250), QSize(200,50)));
    addAllPointsButton->setGeometry(QRect(QPoint(100,325), QSize(200,50)));
    testButton->setGeometry(QRect(QPoint(100,325), QSize(200,50)));
    testSettingsButton->setGeometry(QRect(QPoint(100,325), QSize(200,50)));
    resetButton->setGeometry(QRect(QPoint(100,325), QSize(200,50)));

    connect(openButton, SIGNAL(released()), this, SLOT(open()));
    connect(saveButton, SIGNAL(released()), this, SLOT(save()));
    connect(switchButton, SIGNAL(released()), this, SLOT(switchIm()));
    connect(saliencyButton, SIGNAL(released()), this, SLOT(saliency()));
    connect(stepButton, SIGNAL(released()), this, SLOT(step()));
    connect(step10Button, SIGNAL(released()), this, SLOT(step10()));
    connect(addAllPointsButton, SIGNAL(released()), this, SLOT(addAllPoints()));
    connect(testButton, SIGNAL(released()), this, SLOT(test()));
    connect(testSettingsButton, SIGNAL(released()), this, SLOT(testSettings()));
    connect(resetButton, SIGNAL(released()), this, SLOT(reset()));

    menu_layout->addWidget(openButton);
    menu_layout->addWidget(saveButton);
    menu_layout->addWidget(switchButton);
    menu_layout->addWidget(saliencyButton);
    menu_layout->addWidget(stepButton);
    menu_layout->addWidget(step10Button);
    menu_layout->addWidget(addAllPointsButton);
    menu_layout->addWidget(testButton);
    menu_layout->addWidget(testSettingsButton);
    menu_layout->addWidget(resetButton);

    main_layout->addLayout(menu_layout, 1);
    main_layout->addWidget(image_area, 3);
    main_layout->addWidget(opengl_area, 3);

    setLayout(main_layout);
}

void Application::open()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                               tr("Open File"), QDir::currentPath());
    if (!fileName.isEmpty()) {
        image_area->openImage(fileName);
        image_area->setN_xy();
        opengl_area->update();
    }
}

void Application::save()
{
    QAction *action = qobject_cast<QAction *>(sender());
    QByteArray fileFormat = action->data().toByteArray();
    saveFile(fileFormat);
}

void Application::step()
{
    image_area->triangulate();
    opengl_area->update();
}

void Application::step10()
{
    for(int i=0 ; i<10 ; i++){
        step();
    }
}

void Application::switchIm(){
    image_area->switchImage();
}

void Application::saliency(){
    dialog = new SaliencyForm();
    dialog->linkTrianguledImage(image_area);
    dialog->exec();
}

void Application::addAllPoints(){
    image_area->addPoints();
    opengl_area->update();
}

void Application::test(){
    image_area->reset();
    image_area->test();
    opengl_area->update();
}

void Application::testSettings(){
    test_form = new testForm();
    test_form->linkTrianguledImage(image_area);
    test_form->exec();
}

void Application::reset(){
    image_area->reset();
    opengl_area->update();
}


bool Application::saveFile(const QByteArray &fileFormat)
{
    QString initialPath = QDir::currentPath() + "/untitled." + fileFormat;

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                               initialPath,
                               tr("%1 Files (*.%2);;All Files (*)")
                               .arg(QString::fromLatin1(fileFormat.toUpper()))
                               .arg(QString::fromLatin1(fileFormat)));
    if (fileName.isEmpty()) {
        return false;
    } else {
        return image_area->saveImage(fileName, fileFormat.constData());
    }
}


