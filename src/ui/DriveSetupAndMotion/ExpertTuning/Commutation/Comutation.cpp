#include "Comutation.h"
#include "ui_Comutation.h"

Comutation::Comutation(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Comutation)
{
    ui->setupUi(this);
}

Comutation::~Comutation()
{
    delete ui;
}
