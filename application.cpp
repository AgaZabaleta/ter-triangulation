#include <QtWidgets>

#include "application.h"
#include "trianguled_image.h"

//! [0]
Application::Application(QWidget* parent) :
    QWidget(parent),
    image_area(new Trianguled_image(10, 10, this)),
    opengl_area(new OpenGLTriangles(this))
{
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    main_layout = new QHBoxLayout();
    menu_layout = new QVBoxLayout();

    openButton = new QPushButton("Open", this);
    saveButton = new QPushButton("Save", this);
    greyButton = new QPushButton("Shades of grey", this);
    stepButton = new QPushButton("Step", this);
    addPointButton = new QPushButton("Add point", this);
    addAllPointsButton = new QPushButton("Add all points", this);

    openButton->setGeometry(QRect(QPoint(100,100), QSize(200,50)));
    saveButton->setGeometry(QRect(QPoint(100,175), QSize(200,50)));
    greyButton->setGeometry(QRect(QPoint(100,175), QSize(200,50)));
    stepButton->setGeometry(QRect(QPoint(100,250), QSize(200,50)));
    addPointButton->setGeometry(QRect(QPoint(100,325), QSize(200,50)));
    addAllPointsButton->setGeometry(QRect(QPoint(100,325), QSize(200,50)));

    connect(openButton, SIGNAL(released()), this, SLOT(open()));
    connect(saveButton, SIGNAL(released()), this, SLOT(save()));
    connect(greyButton, SIGNAL(released()), this, SLOT(grey()));
    connect(stepButton, SIGNAL(released()), this, SLOT(step()));
    connect(addPointButton, SIGNAL(released()), this, SLOT(addPoint()));
    connect(addAllPointsButton, SIGNAL(released()), this, SLOT(addAllPoints()));

    menu_layout->addWidget(openButton);
    menu_layout->addWidget(saveButton);
    menu_layout->addWidget(greyButton);
    menu_layout->addWidget(stepButton);
    menu_layout->addWidget(addPointButton);
    menu_layout->addWidget(addAllPointsButton);

    main_layout->addLayout(menu_layout, 1);
    main_layout->addWidget(image_area, 3);
    main_layout->addWidget(opengl_area, 3);

    setLayout(main_layout);

}

void Application::open()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                               tr("Open File"), QDir::currentPath());
    if (!fileName.isEmpty())
        image_area->openImage(fileName);
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
}

void Application::grey(){
    image_area->transformToGrey();
}

void Application::addPoint()
{
    image_area->addRandomPoint();
}

void Application::addAllPoints(){
    image_area->addPoints();
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
