#include "CurrentLimitsController.h"
#include "../src/core/CommandDispatcher/CommandDispatcher.h"
#include "src/core/CommunicationManager/Model/Command.h"
#include "src/core/CommunicationManager/Model/Response.h"
#include "LogManager.h"
#include "BusinessStateMachineContext.h"
#include "../core/CommandDispatcher/PeriodicQueryManager.h"

CurrentLimitsController::CurrentLimitsController(sp<BusinessStateMachineContext> context, sp<CommandDispatcher> commandDispatcher_, std::shared_ptr<spdlog::logger> logger_, QObject *parent)
{
    setAll(context,logger_,commandDispatcher_);
    m_sequentialProcessor.reset(new SequentialCommandProcessor(commandDispatcher(), logger(),this));
    setupConnections();
}

void CurrentLimitsController::queryParameters()
{
    // PL[1] komutu
    auto pl1Command = std::make_shared<Command>("PL", 1, CommandType::Query);
    m_sequentialProcessor->addCommand(pl1Command, [this](std::shared_ptr<Response> response) {
        if (response->isSuccess()) {
            QMetaObject::invokeMethod(this, [this, response]() {
                emit changedPeakCurrentValue(response->rawResponse());
            }, Qt::QueuedConnection);
        } else {
            QMetaObject::invokeMethod(this, [this, errorMsg = response->errorMessage()]() {
                SpdlogError(logger().get(), "Error: {}" + errorMsg.toStdString());
            }, Qt::QueuedConnection);
        }
    }, 1000);

    // PL[2] komutu
    auto pl2Command = std::make_shared<Command>("PL", 2, CommandType::Query);
    m_sequentialProcessor->addCommand(pl2Command, [this](std::shared_ptr<Response> response) {
        if (response->isSuccess()) {
            QMetaObject::invokeMethod(this, [this, response]() {
                emit changedPeakCurrentDuration(response->rawResponse());
            }, Qt::QueuedConnection);
        } else {
            QMetaObject::invokeMethod(this, [this, errorMsg = response->errorMessage()]() {
                SpdlogError(logger().get(), "Error: {}" + errorMsg.toStdString());
            }, Qt::QueuedConnection);
        }
    }, 1000);

    // CL[1] komutu
    auto cl1Command = std::make_shared<Command>("CL", 1, CommandType::Query);
    m_sequentialProcessor->addCommand(cl1Command, [this](std::shared_ptr<Response> response) {
        if (response->isSuccess()) {
            QMetaObject::invokeMethod(this, [this, response]() {
                emit changedPeakContinuousCurrent(response->rawResponse());
            }, Qt::QueuedConnection);
        } else {
            QMetaObject::invokeMethod(this, [this, errorMsg = response->errorMessage()]() {
                SpdlogError(logger().get(), "Error: {}" + errorMsg.toStdString());
            }, Qt::QueuedConnection);
        }
    }, 1000);

    // US[1] komutu
    auto us1Command = std::make_shared<Command>("US", 1, CommandType::Query);
    m_sequentialProcessor->addCommand(us1Command, [this](std::shared_ptr<Response> response) {
        if (response->isSuccess()) {
            QMetaObject::invokeMethod(this, [this, response]() {
                emit changedPeakPWMOutputDutyCycle(response->rawResponse());
            }, Qt::QueuedConnection);
        } else {
            QMetaObject::invokeMethod(this, [this, errorMsg = response->errorMessage()]() {
                SpdlogError(logger().get(), "Error: {}" + errorMsg.toStdString());
            }, Qt::QueuedConnection);
        }
    }, 1000);

    // US[2] komutu
    auto us2Command = std::make_shared<Command>("US", 2, CommandType::Query);
    m_sequentialProcessor->addCommand(us2Command, [this](std::shared_ptr<Response> response) {
        if (response->isSuccess()) {
            QMetaObject::invokeMethod(this, [this, response]() {
                emit changedPeakIntegralLimit(response->rawResponse());
            }, Qt::QueuedConnection);
        } else {
            QMetaObject::invokeMethod(this, [this, errorMsg = response->errorMessage()]() {
                SpdlogError(logger().get(), "Error: {}" + errorMsg.toStdString());
            }, Qt::QueuedConnection);
        }
    }, 1000);

    // US[4] komutu
    auto us4Command = std::make_shared<Command>("US", 4, CommandType::Query);
    m_sequentialProcessor->addCommand(us4Command, [this](std::shared_ptr<Response> response) {
        if (response->isSuccess()) {
            QMetaObject::invokeMethod(this, [this, response]() {
                emit changedPeakMaxVoltageBV(response->rawResponse());
            }, Qt::QueuedConnection);
        } else {
            QMetaObject::invokeMethod(this, [this, errorMsg = response->errorMessage()]() {
                SpdlogError(logger().get(), "Error: {}" + errorMsg.toStdString());
            }, Qt::QueuedConnection);
        }
    }, 1000);
}

void CurrentLimitsController::setupConnections()
{
    connect(businessStateMachineContext().get(), &BusinessStateMachineContext::driveSetupAndMotionStateEntered, this, [&](){
        auto& queryManager = PeriodicQueryManager::instance();

        // Use it as before
        queryManager.registerQuery(
            "current_page_pl1",
            "current_settings_page",
            std::make_shared<Command>("SR", -1, CommandType::Query),
            [this](std::shared_ptr<Response> response) {
                SpdlogInfo(logger().get(), "PL[1] response received" + response->rawResponse().toStdString());
            },
            10
            );
        queryManager.startQueryGroup("current_settings_page");
    });

    connect(businessStateMachineContext().get(), &BusinessStateMachineContext::driveSetupAndMotionStateExited, this, [&](){

        auto& queryManager = PeriodicQueryManager::instance();
        queryManager.stopQueryGroup("current_settings_page");
    });
}
