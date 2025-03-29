#include "VerticalMenuBarController.h"
#include "WorkspaceWidget.h"

VerticalMenuBarController::VerticalMenuBarController(std::shared_ptr<spdlog::logger> logger_, VerticalMenuBar * verticalMenuBar_, QObject *parent) :
    QObject{parent},
    m_Logger{logger_},
    m_VerticalMenuBar{verticalMenuBar_}
{
    setupConnections();
}

void VerticalMenuBarController::setupConnections()
{
    connect(m_VerticalMenuBar->getWorkspaceWidget(),&WorkspaceWidget::addGoldDrive, this, &VerticalMenuBarController::addGoldDrive);
    connect(m_VerticalMenuBar->getWorkspaceWidget(),&WorkspaceWidget::removeSelectedDrive, this, &VerticalMenuBarController::removeSelectedDrive);
}
