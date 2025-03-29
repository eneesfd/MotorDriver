#include "Preferences.h"
#include "ui_Preferences.h"

Preferences::Preferences(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Preferences)
{
    ui->setupUi(this);
    m_settingsDialog.reset(new SettingsDialog(this));
    connect(ui->pushButton, &QPushButton::clicked, this, &Preferences::backButtonClicked);
    connect(ui->pushButton_6, &QPushButton::clicked, m_settingsDialog.get(), &SettingsDialog::open);
}

Preferences::~Preferences()
{
    delete ui;
}
