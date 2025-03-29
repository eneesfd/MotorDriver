#ifndef GOLDDRIVEMOTOR_H
#define GOLDDRIVEMOTOR_H

#include "IMotorDriver.h"

namespace MotorDriver {

class GoldDriveMotor : public IMotorDriver
{
public:
    GoldDriveMotor();
    ~GoldDriveMotor() override;

    void connect() override;
    void disconnect() override;
    void sendCommand() override;
    QString receiveResponse() override;
    void start() override;
    void stop() override;
};

}


#endif // GOLDDRIVEMOTOR_H
