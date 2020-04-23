#include "testform.h"
#include "ui_testform.h"

testForm::testForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::testForm)
{
    ui->setupUi(this);
}

void testForm::linkTrianguledImage(Trianguled_image* t_i){
    t_image = t_i;
}

testForm::~testForm()
{
    delete ui;
}

void testForm::on_buttonBox_accepted()
{
    double percent = 1.0 / static_cast<double>(ui->n_points->value());
    t_image->testSettings(percent, ui->gradient_v->value(), ui->color_v->value(), ui->texture_v->value(), ui->recurrent_v->isChecked(), ui->laplacian_v->value());
}
