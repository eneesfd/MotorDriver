#include "CurrentMode.h"
#include "ui_CurrentMode.h"

CurrentMode::CurrentMode(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CurrentMode)
{
    ui->setupUi(this);
}

CurrentMode::~CurrentMode()
{
    delete ui;
}
