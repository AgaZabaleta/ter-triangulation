#ifndef SALIENCYFORM_H
#define SALIENCYFORM_H

#include <QDialog>
#include <QtWidgets>
#include <trianguled_image.h>

namespace Ui {
class SaliencyForm;
}

class SaliencyForm : public QDialog
{
    Q_OBJECT

public:
    explicit SaliencyForm(QWidget *parent = nullptr);
    ~SaliencyForm();
    void linkTrianguledImage(Trianguled_image*);

private slots:
    void on_buttonBox_accepted();

private:
    Ui::SaliencyForm *ui;
    Trianguled_image *t_image;
};

#endif // SALIENCYFORM_H
