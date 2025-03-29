#include "VelocityAndPositionVerificationTime.h"
#include "ui_VelocityAndPositionVerificationTime.h"

VelocityAndPositionVerificationTime::VelocityAndPositionVerificationTime(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::VelocityAndPositionVerificationTime)
{
    ui->setupUi(this);
}

VelocityAndPositionVerificationTime::~VelocityAndPositionVerificationTime()
{
    delete ui;
}
