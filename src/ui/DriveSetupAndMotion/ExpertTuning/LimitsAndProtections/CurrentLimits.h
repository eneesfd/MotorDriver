#ifndef CURRENTLIMITS_H
#define CURRENTLIMITS_H

#include <QWidget>

namespace Ui {
class CurrentLimits;
}

class CurrentLimitsController;

class CurrentLimits : public QWidget
{
    Q_OBJECT

public:
    explicit CurrentLimits(QWidget *parent = nullptr);
    ~CurrentLimits();
    void setControllers(QSharedPointer<CurrentLimitsController> controller);
private slots:
    void onChangedPeakCurrentValue(QString value);
    void onChangedPeakCurrentDuration(QString value);
    void onChangedPeakContinuousCurrent(QString value);
    void onChangedPeakPWMOutputDutyCycle(QString value);
    void onChangedPeakMaxVoltageBV(QString value);
    void onChangedPeakIntegralLimit(QString value);
private:
    Ui::CurrentLimits *ui;
    QSharedPointer<CurrentLimitsController> m_controller;
};

#endif // CURRENTLIMITS_H
