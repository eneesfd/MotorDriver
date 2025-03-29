#include "LoadMotorDialog.h"
#include "ui_LoadMotorDialog.h"

LoadMotorDialog::LoadMotorDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoadMotorDialog)
{
    ui->setupUi(this);
    createSignalsAndSlots();
    setLineEditsValidators();
}

LoadMotorDialog::~LoadMotorDialog()
{
    delete ui;
}

void LoadMotorDialog::createSignalsAndSlots()
{
    // GroupBox Connections
    connect(ui->grpbxGeneral,       &QGroupBox::toggled, this, &LoadMotorDialog::onGroupBoxToggled);
    connect(ui->grpbxMotorSettings, &QGroupBox::toggled, this, &LoadMotorDialog::onGroupBoxToggled);
    connect(ui->grpbxMotorData,     &QGroupBox::toggled, this, &LoadMotorDialog::onGroupBoxToggled);
}

void LoadMotorDialog::onGroupBoxToggled(bool checked)
{
    // Bu fonksiyonun hangi QGroupBox'tan çağrıldığını alıyoruz
    QGroupBox* groupBox = qobject_cast<QGroupBox*>(sender());
    if (!groupBox) return; // Eğer sender bir QGroupBox değilse çık

    // Sadece ilgili groupBox içindeki widget'ları görünür/görünmez yap
    for (auto* child : groupBox->findChildren<QWidget*>()) {
        child->setVisible(checked);
    }
}

void LoadMotorDialog::setLineEditsValidators()
{
    QRegularExpression regex("-?\\d*(\\.\\d*)?");
    QRegularExpressionValidator *validator = new QRegularExpressionValidator(regex);

    ui->ldtPeakCurrent->setValidator(validator);
    ui->ldtContinuousStallCurrent->setValidator(validator);
    ui->ldtMaximalMotorSpeed->setValidator(validator);
    ui->ldtRatedTorque->setValidator(validator);
    ui->ldtPeakTorque->setValidator(validator);
    ui->ldtBEMFConstatnKe->setValidator(validator);
    ui->ldtKt->setValidator(validator);
    ui->ldtMotorResistancePhaseToPhase->setValidator(validator);
    ui->ldtMotorInductancePhaseToPhase->setValidator(validator);
    ui->ldtNumberOfPolePairs->setValidator(validator);
    ui->ldtRatedTorqueSpeed->setValidator(validator);
    ui->ldtZeroTorqueSpeed->setValidator(validator);
}
