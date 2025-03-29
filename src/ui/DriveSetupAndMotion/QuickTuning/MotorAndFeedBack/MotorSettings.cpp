#include "MotorSettings.h"
#include "ui_MotorSettings.h"

MotorSettings::MotorSettings(QWidget *parent)
    : QWidget(parent),
    mLoadMotorDialog{new LoadMotorDialog},
    ui(new Ui::MotorSettings)
{
    ui->setupUi(this);
    setLineEditsValidators();
    createSignalsAndSlots();
}

MotorSettings::~MotorSettings()
{
    delete ui;
}

void MotorSettings::setLineEditsValidators()
{
    QRegularExpression regex("-?\\d*(\\.\\d*)?");
    QRegularExpressionValidator *validator = new QRegularExpressionValidator(regex);

    ui->ldtPeakCurrent->setValidator(validator);
    ui->ldtContinuousPeakCurrent->setValidator(validator);
    ui->ldtMaximalMotorSpeed->setValidator(validator);
    ui->ldtPolePairsPerRevolution->setValidator(validator);
    ui->ldtRresistancePhaseToPhase->setValidator(validator);
    ui->ldtLinductancePhaseToPhase->setValidator(validator);
    ui->ldtKeBackEmfConstant->setValidator(validator);
}

void MotorSettings::createSignalsAndSlots()
{
    connect(ui->btnLoadMotor, &QPushButton::clicked, mLoadMotorDialog, &LoadMotorDialog::show);
}
