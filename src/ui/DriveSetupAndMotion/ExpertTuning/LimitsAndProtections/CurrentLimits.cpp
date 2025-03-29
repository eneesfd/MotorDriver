#include "CurrentLimits.h"
#include "ui_CurrentLimits.h"
#include "../controllers/DriveSetupAndMotion/ExpertTuning/LimitsAndProtections/CurrentLimitsController.h"

CurrentLimits::CurrentLimits(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CurrentLimits)
{
    ui->setupUi(this);
}

CurrentLimits::~CurrentLimits()
{
    delete ui;
}

void CurrentLimits::setControllers(QSharedPointer<CurrentLimitsController> controller)
{
    m_controller = controller;
    connect(m_controller.data(), &CurrentLimitsController::changedPeakCurrentValue, this, &CurrentLimits::onChangedPeakCurrentValue);
    connect(m_controller.data(), &CurrentLimitsController::changedPeakCurrentDuration, this, &CurrentLimits::onChangedPeakCurrentDuration);
    connect(m_controller.data(), &CurrentLimitsController::changedPeakContinuousCurrent, this, &CurrentLimits::onChangedPeakContinuousCurrent);
    connect(m_controller.data(), &CurrentLimitsController::changedPeakPWMOutputDutyCycle, this, &CurrentLimits::onChangedPeakPWMOutputDutyCycle);
    connect(m_controller.data(), &CurrentLimitsController::changedPeakMaxVoltageBV, this, &CurrentLimits::onChangedPeakMaxVoltageBV);
    connect(m_controller.data(), &CurrentLimitsController::changedPeakIntegralLimit, this, &CurrentLimits::onChangedPeakIntegralLimit);
}

void CurrentLimits::onChangedPeakCurrentValue(QString value)
{
    ui->ldtPeakCurrent->setText(value);
}

void CurrentLimits::onChangedPeakCurrentDuration(QString value)
{
    ui->ldtPeakCurrentDuration->setText(value);
}

void CurrentLimits::onChangedPeakContinuousCurrent(QString value)
{
    ui->ldtContinousCurrent->setText(value);
}

void CurrentLimits::onChangedPeakPWMOutputDutyCycle(QString value)
{
    ui->ldtPWMOutput->setText(value);
}

void CurrentLimits::onChangedPeakMaxVoltageBV(QString value)
{
    ui->ldtMaxVoltage->setText(value);
}

void CurrentLimits::onChangedPeakIntegralLimit(QString value)
{
    ui->ldtIntegralLimit->setText(value);
}
