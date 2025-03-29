#ifndef LOADMOTORDIALOG_H
#define LOADMOTORDIALOG_H

#include <QDialog>

namespace Ui {
class LoadMotorDialog;
}

class LoadMotorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoadMotorDialog(QWidget *parent = nullptr);
    ~LoadMotorDialog();

private:
    void createSignalsAndSlots();
    void onGroupBoxToggled(bool checked);
    void setLineEditsValidators();
    Ui::LoadMotorDialog *ui;
};

#endif // LOADMOTORDIALOG_H
