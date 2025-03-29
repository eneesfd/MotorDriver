#include "StepperClosedLoop.h"
#include "ui_StepperClosedLoop.h"

StepperClosedLoop::StepperClosedLoop(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StepperClosedLoop)
{
    ui->setupUi(this);
}

StepperClosedLoop::~StepperClosedLoop()
{
    delete ui;
}
