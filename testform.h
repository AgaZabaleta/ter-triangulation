#ifndef TESTFORM_H
#define TESTFORM_H

#include <QDialog>
#include <QtWidgets>
#include <trianguled_image.h>

namespace Ui {
class testForm;
}

class testForm : public QDialog
{
    Q_OBJECT

public:
    explicit testForm(QWidget *parent = nullptr);
    ~testForm();
    void linkTrianguledImage(Trianguled_image*);

private slots:
    void on_buttonBox_accepted();

private:
    Ui::testForm *ui;
    Trianguled_image *t_image;
};

#endif // TESTFORM_H
