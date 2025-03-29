#include "MotionSingleAxis.h"
#include "ui_MotionSingleAxis.h"

MotionSingleAxis::MotionSingleAxis(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MotionSingleAxis)
{
    ui->setupUi(this);
}

MotionSingleAxis::~MotionSingleAxis()
{
    delete ui;
}

void MotionSingleAxis::setUIStateMachine(QSharedPointer<UIStateMachineController> stateMachine)
{
    ui->wgtSingleAxisMotion->setUIStateMachine(stateMachine);
}

void MotionSingleAxis::setLogManager(std::shared_ptr<spdlog::logger> log_)
{
    m_logManager = log_;
    ui->wgtSingleAxisMotion->setLogManager(log_);
}
