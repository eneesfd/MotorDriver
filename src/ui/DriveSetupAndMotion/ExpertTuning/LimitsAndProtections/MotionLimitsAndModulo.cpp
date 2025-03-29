#include "MotionLimitsAndModulo.h"
#include "ui_MotionLimitsAndModulo.h"

MotionLimitsAndModulo::MotionLimitsAndModulo(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MotionLimitsAndModulo)
{
    ui->setupUi(this);
}

MotionLimitsAndModulo::~MotionLimitsAndModulo()
{
    delete ui;
}
