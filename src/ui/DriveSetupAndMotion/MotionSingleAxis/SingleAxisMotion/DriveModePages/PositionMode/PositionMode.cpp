#include "PositionMode.h"
#include "ui_PositionMode.h"

PositionMode::PositionMode(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PositionMode)
{
    ui->setupUi(this);
}

PositionMode::~PositionMode()
{
    delete ui;
}
