#include "CurrentDesign.h"
#include "ui_CurrentDesign.h"

CurrentDesign::CurrentDesign(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CurrentDesign)
{
    ui->setupUi(this);
}

CurrentDesign::~CurrentDesign()
{
    delete ui;
}
