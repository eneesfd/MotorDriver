#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"
#include "QtAdvancedStylesheet.h"
#include "EventAggregator.h"

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    fillThemeCombobox();
    setupConnections();
}

void SettingsDialog::setupConnections()
{
    connect(ui->cmbChangeColorTheme, &QComboBox::currentTextChanged, &EventAggregator::instance(), &EventAggregator::changeApplicationTheme);
}


SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::fillThemeCombobox()
{
    acss::QtAdvancedStylesheet advancedStyleSheet_;
    QString AppDir = qApp->applicationDirPath();
    QString StylesDir = AppDir + "/styles";
    advancedStyleSheet_.setOutputDirPath(AppDir + "/output");
    advancedStyleSheet_.setStylesDirPath(StylesDir);

    advancedStyleSheet_.setCurrentStyle("motorDriver_material");
    advancedStyleSheet_.setCurrentTheme("light_amber");

    for (const auto& theme : advancedStyleSheet_.themes()) {
        ui->cmbChangeColorTheme->addItem(theme);
    }
}

