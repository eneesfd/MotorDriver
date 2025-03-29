#ifndef MOTORDRIVERFACTORY_H
#define MOTORDRIVERFACTORY_H

#include "Constants.h"
#include "GoldDriveMotor.h"
#include "PlatiniumDriveMotor.h"

namespace MotorDriver {

class MotorDriverFactory {
public:
    inline std::unique_ptr<IMotorDriver> createMotorDriver(Constants::MotorDriverTypes::MotorDriverType type)
    {
        switch (type) {
        case Constants::MotorDriverTypes::MotorDriverType::PlatiniumDriveMotor:
            return std::make_unique<PlatiniumDriveMotor>();
        case Constants::MotorDriverTypes::MotorDriverType::GoldDriveMotor:
            return std::make_unique<GoldDriveMotor>();
        default:
            return nullptr;
        }
    }
};
}
#endif // MOTORDRIVERFACTORY_H
