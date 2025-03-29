#include "StoppingOptions.h"
#include "ui_StoppingOptions.h"

StoppingOptions::StoppingOptions(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StoppingOptions)
{
    ui->setupUi(this);
}

StoppingOptions::~StoppingOptions()
{
    delete ui;
}
