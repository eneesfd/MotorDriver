#ifndef MOTORSETTINGS_H
#define MOTORSETTINGS_H

#include <QWidget>
#include <QGroupBox>
#include "LoadMotorDialog.h"

namespace Ui {
class MotorSettings;
}

class MotorSettings : public QWidget
{
    Q_OBJECT

public:
    explicit MotorSettings(QWidget *parent = nullptr);
    ~MotorSettings();

private:
    void setLineEditsValidators();
    void createSignalsAndSlots();
    Ui::MotorSettings *ui;
    LoadMotorDialog *mLoadMotorDialog;
};

#endif // MOTORSETTINGS_H
