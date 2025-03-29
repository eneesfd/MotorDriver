#include "VelocityAndPositionDesign.h"
#include "ui_VelocityAndPositionDesign.h"

VelocityAndPositionDesign::VelocityAndPositionDesign(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::VelocityAndPositionDesign)
{
    ui->setupUi(this);
}

VelocityAndPositionDesign::~VelocityAndPositionDesign()
{
    delete ui;
}
