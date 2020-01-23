#include <QtWidgets>

#include "mainwindow.h"
#include "application.h"

//! [0]
MainWindow::MainWindow(QWidget* parent):QMainWindow(parent), application(new Application(this))
{
    setCentralWidget(application);
    resize(666,500);
    setWindowTitle(tr("Triangulation d'image"));
}
