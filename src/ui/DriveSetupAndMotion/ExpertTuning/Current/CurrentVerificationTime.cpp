#include "CurrentVerificationTime.h"
#include "ui_CurrentVerificationTime.h"

CurrentVerificationTime::CurrentVerificationTime(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CurrentVerificationTime)
{
    ui->setupUi(this);
}

CurrentVerificationTime::~CurrentVerificationTime()
{
    delete ui;
}
