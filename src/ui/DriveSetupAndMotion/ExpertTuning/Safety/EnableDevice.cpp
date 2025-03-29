#include "EnableDevice.h"
#include "ui_EnableDevice.h"

EnableDevice::EnableDevice(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::EnableDevice)
{
    ui->setupUi(this);
}

EnableDevice::~EnableDevice()
{
    delete ui;
}
