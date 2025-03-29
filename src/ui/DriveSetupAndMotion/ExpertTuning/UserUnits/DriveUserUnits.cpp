#include "DriveUserUnits.h"
#include "ui_DriveUserUnits.h"

DriveUserUnits::DriveUserUnits(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DriveUserUnits)
{
    ui->setupUi(this);
}

DriveUserUnits::~DriveUserUnits()
{
    delete ui;
}
