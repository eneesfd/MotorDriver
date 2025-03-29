#include "HomingMode.h"
#include "ui_HomingMode.h"

HomingMode::HomingMode(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HomingMode)
{
    ui->setupUi(this);
}

HomingMode::~HomingMode()
{
    delete ui;
}
