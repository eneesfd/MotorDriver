#include "StepperMode.h"
#include "ui_StepperMode.h"

StepperMode::StepperMode(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StepperMode)
{
    ui->setupUi(this);
}

StepperMode::~StepperMode()
{
    delete ui;
}
