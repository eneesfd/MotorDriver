#include "Brake.h"
#include "ui_Brake.h"

Brake::Brake(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Brake)
{
    ui->setupUi(this);
    setLabelValidetor();
}

Brake::~Brake()
{
    delete ui;
}

void Brake::setLabelValidetor()
{
    QRegularExpression regex("-?\\d*(\\.\\d*)?");
    QRegularExpressionValidator *validator = new QRegularExpressionValidator(regex);

    ui->ldtBrakeEngageTime->setValidator(validator);
    ui->ldtBrakeReleaseTime->setValidator(validator);
    ui->ldtDynamicBrakeSpeedThreshold->setValidator(validator);
}

