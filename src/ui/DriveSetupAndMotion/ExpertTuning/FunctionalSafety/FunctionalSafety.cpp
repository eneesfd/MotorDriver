#include "FunctionalSafety.h"
#include "ui_FunctionalSafety.h"

FunctionalSafety::FunctionalSafety(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FunctionalSafety)
{
    ui->setupUi(this);
}

FunctionalSafety::~FunctionalSafety()
{
    delete ui;
}
