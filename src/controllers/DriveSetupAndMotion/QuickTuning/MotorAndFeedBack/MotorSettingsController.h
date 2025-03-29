#ifndef MOTORSETTINGSCONTROLLER_H
#define MOTORSETTINGSCONTROLLER_H

#include <QObject>

class MotorSettingsController : public QObject
{
    Q_OBJECT
public:
    explicit MotorSettingsController(QObject *parent = nullptr);

signals:
};

#endif // MOTORSETTINGSCONTROLLER_H
