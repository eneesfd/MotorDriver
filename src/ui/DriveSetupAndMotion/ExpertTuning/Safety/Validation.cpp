#include "Validation.h"
#include "ui_Validation.h"

Validation::Validation(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Validation)
{
    ui->setupUi(this);
}

Validation::~Validation()
{
    delete ui;
}
