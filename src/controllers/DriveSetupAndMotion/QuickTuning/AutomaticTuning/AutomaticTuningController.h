#ifndef AUTOMATICTUNINGCONTROLLER_H
#define AUTOMATICTUNINGCONTROLLER_H

#include <QObject>

class AutomaticTuningController : public QObject
{
    Q_OBJECT
public:
    explicit AutomaticTuningController(QObject *parent = nullptr);

signals:
};

#endif // AUTOMATICTUNINGCONTROLLER_H
