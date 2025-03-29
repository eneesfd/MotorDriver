#include "DisplayUserUnits.h"
#include "ui_DisplayUserUnits.h"

DisplayUserUnits::DisplayUserUnits(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DisplayUserUnits)
{
    ui->setupUi(this);
}

DisplayUserUnits::~DisplayUserUnits()
{
    delete ui;
}
