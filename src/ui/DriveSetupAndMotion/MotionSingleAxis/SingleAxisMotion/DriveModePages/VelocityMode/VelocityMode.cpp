#include "VelocityMode.h"
#include "ui_VelocityMode.h"

VelocityMode::VelocityMode(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::VelocityMode)
{
    ui->setupUi(this);
}

VelocityMode::~VelocityMode()
{
    delete ui;
}
