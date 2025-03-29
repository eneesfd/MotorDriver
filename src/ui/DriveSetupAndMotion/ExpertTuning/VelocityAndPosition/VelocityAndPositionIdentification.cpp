#include "VelocityAndPositionIdentification.h"
#include "ui_VelocityAndPositionIdentification.h"

VelocityAndPositionIdentification::VelocityAndPositionIdentification(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::VelocityAndPositionIdentification)
{
    ui->setupUi(this);
    initializeFormSettings();
    createSignalsAndSlots();
}

VelocityAndPositionIdentification::~VelocityAndPositionIdentification()
{
    delete ui;
}

void VelocityAndPositionIdentification::initializeFormSettings()
{
    ui->tabWidget->setTabVisible(1, false);
    ui->tabWidget->setTabVisible(2, false);

    // QPushButton'ı burada oluştur ve konumlandır
    btnShowHideTabs = new QPushButton("Advanced", ui->tabWidget);
    int buttonX = ui->tabWidget->width() - btnShowHideTabs->width() - 5;
    int buttonY = -3;
    btnShowHideTabs->move(buttonX + 400 - 75 , buttonY);
    btnShowHideTabs->setCheckable(true);
}

void VelocityAndPositionIdentification::createSignalsAndSlots()
{
    connect(btnShowHideTabs, &QPushButton::toggled, [&](bool checked) {
        if (checked) {
            ui->tabWidget->setTabVisible(1, true);
            ui->tabWidget->setTabVisible(2, true);
            btnShowHideTabs->setText("Hide");
        } else {
            ui->tabWidget->setTabVisible(1, false);
            ui->tabWidget->setTabVisible(2, false);
            btnShowHideTabs->setText("Advanced");
        }
    });
}
