#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QPushButton>
#include <QWidget>
#include <Settings/SettingsDialog.h>
#include <spdlog/spdlog.h>

namespace Ui {
class Preferences;
}

class Preferences : public QWidget
{
    Q_OBJECT

public:
    explicit Preferences(QWidget *parent = nullptr);
    ~Preferences();
    QPushButton* getButton();
    QPushButton* getSettingButton();
    QSharedPointer<SettingsDialog> m_settingsDialog;
    QSharedPointer<SettingsDialog> settingsDialog() const;
signals:
    void backButtonClicked();
private:
    Ui::Preferences *ui;
};

#endif // PREFERENCES_H
