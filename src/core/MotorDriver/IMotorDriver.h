#ifndef IMOTORDRIVER_H
#define IMOTORDRIVER_H

#include <QString>

namespace MotorDriver {

class IMotorDriver
{
public:
    virtual ~IMotorDriver() = default;

    virtual void connect() = 0;
    virtual void disconnect() = 0;
    virtual void sendCommand() = 0;
    virtual QString receiveResponse() = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
};

}

#endif // IMOTORDRIVER_H
