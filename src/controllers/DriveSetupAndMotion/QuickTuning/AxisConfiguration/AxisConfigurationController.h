#ifndef AXISCONFIGURATIONCONTROLLER_H
#define AXISCONFIGURATIONCONTROLLER_H

#include <QObject>

class AxisConfigurationController : public QObject
{
    Q_OBJECT
public:
    explicit AxisConfigurationController(QObject *parent = nullptr);

signals:
};

#endif // AXISCONFIGURATIONCONTROLLER_H
