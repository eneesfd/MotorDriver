#include "SafetyIOConfiguration.h"
#include "ui_SafetyIOConfiguration.h"

SafetyIOConfiguration::SafetyIOConfiguration(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SafetyIOConfiguration)
{
    ui->setupUi(this);
}

SafetyIOConfiguration::~SafetyIOConfiguration()
{
    delete ui;
}
