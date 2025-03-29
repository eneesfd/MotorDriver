#ifndef BRAKECONTROLLER_H
#define BRAKECONTROLLER_H

#include <QObject>

class BrakeController : public QObject
{
    Q_OBJECT
public:
    explicit BrakeController(QObject *parent = nullptr);

signals:
};

#endif // BRAKECONTROLLER_H
