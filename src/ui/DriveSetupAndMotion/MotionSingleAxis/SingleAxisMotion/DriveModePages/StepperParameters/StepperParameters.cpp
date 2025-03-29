#include "StepperParameters.h"
#include "ui_StepperParameters.h"

StepperParameters::StepperParameters(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StepperParameters)
{
    ui->setupUi(this);
}

StepperParameters::~StepperParameters()
{
    delete ui;
}
