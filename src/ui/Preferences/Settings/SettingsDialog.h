#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();
signals:
    void themeComboBoxCurrentTextChanged(const QString &currentText_);
private:
    Ui::SettingsDialog *ui;
    void fillThemeCombobox();
    void setupConnections();
};

#endif // SETTINGSDIALOG_H
