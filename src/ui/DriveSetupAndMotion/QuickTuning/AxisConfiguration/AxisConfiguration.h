#ifndef AXISCONFIGURATION_H
#define AXISCONFIGURATION_H

#include <QLineEdit>
#include <QWidget>
#include "CommandManager.h"

namespace Ui {
class AxisConfiguration;
}

class AxisConfiguration : public QWidget
{
    Q_OBJECT

public:
    explicit AxisConfiguration(QWidget *parent = nullptr);
    ~AxisConfiguration();
private slots:
    void on_ldtTotalGearInput_returnPressed();
    void on_chbFeedbackSingle_released();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
private:
    Ui::AxisConfiguration *ui;
    QMap<QWidget*, QString> m_oldTexts;
    QScopedPointer<CommandManager> m_commandManager;
    void setLabelValidetor();
};

#endif // AXISCONFIGURATION_H
