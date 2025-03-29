#ifndef SAFETYIOCONFIGURATION_H
#define SAFETYIOCONFIGURATION_H

#include <QWidget>

namespace Ui {
class SafetyIOConfiguration;
}

class SafetyIOConfiguration : public QWidget
{
    Q_OBJECT

public:
    explicit SafetyIOConfiguration(QWidget *parent = nullptr);
    ~SafetyIOConfiguration();

private:
    Ui::SafetyIOConfiguration *ui;
};

#endif // SAFETYIOCONFIGURATION_H
