#include "SystemConfiguration.h"
#include "ui_SystemConfiguration.h"

SystemConfiguration::SystemConfiguration(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SystemConfiguration)
{
    ui->setupUi(this);
    ui->wgtPanelHeader->getPageTitleLabel()->setText("Item Configuration");
}

SystemConfiguration::~SystemConfiguration()
{
    delete ui;
}

void SystemConfiguration::onWorkSpaceParentItemSelected(QTreeWidgetItem *item)
{
    ui->stckWgtGeneral->setCurrentIndex(0);
}

void SystemConfiguration::onWorkSpaceChildItemSelected(QTreeWidgetItem *item)
{
    ui->stckWgtGeneral->setCurrentIndex(1);
}
