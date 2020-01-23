#include <QtWidgets>

#include "application.h"
#include "trianguled_image.h"

//! [0]
Application::Application(QWidget* parent):QWidget(parent), image_area(new Trianguled_image(this))
{
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    main_layout = new QHBoxLayout();
    menu_layout = new QVBoxLayout();

    image_area = new Trianguled_image;

    openButton = new QPushButton("Open", this);
    saveButton = new QPushButton("Save", this);
    stepButton = new QPushButton("Step", this);
    addPointButton = new QPushButton("Add point", this);

    openButton->setGeometry(QRect(QPoint(100,100), QSize(200,50)));
    saveButton->setGeometry(QRect(QPoint(100,175), QSize(200,50)));
    stepButton->setGeometry(QRect(QPoint(100,250), QSize(200,50)));
    addPointButton->setGeometry(QRect(QPoint(100,325), QSize(200,50)));

    connect(openButton, SIGNAL(released()), this, SLOT(open()));
    connect(saveButton, SIGNAL(released()), this, SLOT(save()));
    connect(stepButton, SIGNAL(released()), this, SLOT(step()));
    connect(addPointButton, SIGNAL(released()), this, SLOT(addPoint()));

    menu_layout->addWidget(openButton);
    menu_layout->addWidget(saveButton);
    menu_layout->addWidget(stepButton);
    menu_layout->addWidget(addPointButton);

    main_layout->addLayout(menu_layout, 1);
    main_layout->addWidget(image_area, 3);

    setLayout(main_layout);

}
//! [0]

//! [3]
void Application::open()
//! [3] //! [4]
{
    QString fileName = QFileDialog::getOpenFileName(this,
                               tr("Open File"), QDir::currentPath());
    if (!fileName.isEmpty())
        image_area->openImage(fileName);
}
//! [4]

//! [5]
void Application::save()
//! [5] //! [6]
{
    QAction *action = qobject_cast<QAction *>(sender());
    QByteArray fileFormat = action->data().toByteArray();
    saveFile(fileFormat);
}
//! [6]

void Application::step()
{
    image_area->triangulate();
}


void Application::addPoint()
{
    image_area->addRandomPoint();
}

//! [19]
bool Application::saveFile(const QByteArray &fileFormat)
//! [19] //! [20]
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
//! [20]
