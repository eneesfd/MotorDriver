#ifndef PLATINIUMDRIVEMOTOR_H
#define PLATINIUMDRIVEMOTOR_H

#include "IMotorDriver.h"

namespace MotorDriver {

class PlatiniumDriveMotor : public IMotorDriver
{
public:
    PlatiniumDriveMotor();
    ~PlatiniumDriveMotor() override;

    void connect() override;
    void disconnect() override;
    void sendCommand() override;
    QString receiveResponse() override;
    void start() override;
    void stop() override;
};

}

#endif // PLATINIUMDRIVEMOTOR_H
