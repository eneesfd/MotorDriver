#include "SingleAxisMotion.h"
#include "ui_SingleAxisMotion.h"
#include "UIStateMachineController.h"
#include "Constants.h"

using namespace Constants::PanelHeader;

SingleAxisMotion::SingleAxisMotion(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SingleAxisMotion)
{
    ui->setupUi(this);
}


SingleAxisMotion::~SingleAxisMotion()
{
    delete ui;
}

void SingleAxisMotion::setUIStateMachine(QSharedPointer<UIStateMachineController> stateMachine)
{
    connect(stateMachine.data(), &UIStateMachineController::motionSingleAxisModeEntered, this, [=, this] () {
        ui->wgtPanelHeader->getWidgetTitleLabel()->setText(tr(MOTION_SINGLE_AXIS_PARAMETERS));
    });
}

void SingleAxisMotion::setLogManager(std::shared_ptr<spdlog::logger> log_)
{
    m_logManager = log_;
}
