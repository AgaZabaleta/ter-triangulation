#include "saliencyform.h"
#include "ui_saliencyform.h"

SaliencyForm::SaliencyForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SaliencyForm)
{
    ui->setupUi(this);
}

SaliencyForm::~SaliencyForm()
{
    delete ui;
}

void SaliencyForm::linkTrianguledImage(Trianguled_image* t_i){
    t_image = t_i;
}

void SaliencyForm::on_buttonBox_accepted()
{
    double a = 0.0;
    double b = 0.0;
    double c = 0.0;
    if(ui->gradient_box->isChecked()){
        a = ui->a_value->value();
    }

    if(ui->color_box->isChecked()){
        b = ui->b_value->value();
    }

    if(ui->texture_box->isChecked()){
        c = ui->c_value->value();
    }
    if(a==0.0 && b==0.0 && c==0.0){
        qInfo("Incorrect values");
    }else{
        t_image->saliency(a, b, c, ui->recurrentPattern->isChecked());
    }
}

