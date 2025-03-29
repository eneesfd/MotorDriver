#include "AxisConfiguration.h"
#include "ui_AxisConfiguration.h"

#include <CheckBoxCommand.h>
#include <LineEditCommand.h>

AxisConfiguration::AxisConfiguration(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AxisConfiguration)
{
    ui->setupUi(this);
    setLabelValidetor();

    foreach (QLineEdit* edit, findChildren<QLineEdit*>()) {
        edit->installEventFilter(this);
    }
    foreach (QCheckBox* edit, findChildren<QCheckBox*>()) {
        edit->installEventFilter(this);
    }
    m_commandManager.reset(new CommandManager());
}

AxisConfiguration::~AxisConfiguration()
{
    delete ui;
}

void AxisConfiguration::setLabelValidetor()
{
    QRegularExpression regex("-?\\d*(\\.\\d*)?");
    QRegularExpressionValidator *validator = new QRegularExpressionValidator(regex);

    ui->ldtTotalGearInput->setValidator(validator);
    ui->ldtTotalGearOutput->setValidator(validator);
    ui->ldtLeadScrew->setValidator(validator);
}

void AxisConfiguration::on_ldtTotalGearInput_returnPressed()
{
    auto cmd = std::make_unique<commands::LineEditCommand>(ui->ldtTotalGearInput, ui->ldtTotalGearInput->text());
    cmd->setOldText(m_oldTexts.value(ui->ldtTotalGearInput, QString()));
    m_oldTexts[ui->ldtTotalGearInput] = ui->ldtTotalGearInput->text();
    m_commandManager->executeCommand(std::move(cmd));
}

bool AxisConfiguration::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::FocusIn) {
        if (auto lineEdit = qobject_cast<QLineEdit*>(obj)) {
            m_oldTexts[lineEdit] = lineEdit->text();
        }
    }   else if (event->type() == QEvent::MouseButtonPress) {
        if (auto checkbox = qobject_cast<QCheckBox*>(obj)) {
            // Tıklama anında checkbox'ın mevcut durumunu sakla.
            m_oldTexts[checkbox] = checkbox->isChecked() ? "true" : "false";
        }
    }
    return QWidget::eventFilter(obj, event);
}

void AxisConfiguration::on_chbFeedbackSingle_released()
{
    auto cmd = std::make_unique<commands::CheckBoxCommand>(ui->chbFeedbackSingle, ui->chbFeedbackSingle->checkState() == Qt::Checked);
    cmd->setOldState(m_oldTexts.value(ui->chbFeedbackSingle) == "true");
    m_oldTexts[ui->chbFeedbackSingle] = ui->chbFeedbackSingle->checkState() == Qt::Checked ? "true" : "false";
    m_commandManager->executeCommand(std::move(cmd));
}

