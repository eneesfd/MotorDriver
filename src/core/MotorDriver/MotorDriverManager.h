#ifndef MOTORDRIVERMANAGER_H
#define MOTORDRIVERMANAGER_H

#include "MotorDriverFactory.h"

namespace spdlog {
class logger;
}

namespace MotorDriver {
class MotorDriverManager
{
public:
    explicit MotorDriverManager(std::shared_ptr<spdlog::logger> logger_) : m_logger(logger_) {};
    void addMotorDriver(int id, Constants::MotorDriverTypes::MotorDriverType type) {
        MotorDriver::MotorDriverFactory factory;
        m_motorDrivers[id] = factory.createMotorDriver(type);
    }

    void removeMotorDriver(int id) {
        m_motorDrivers.erase(id);
    }

    std::shared_ptr<IMotorDriver> getMotorDriver(int id) const {
        auto it = m_motorDrivers.find(id);
        return (it != m_motorDrivers.end()) ? it->second : nullptr;
    }

private:
    std::unordered_map<int, std::shared_ptr<MotorDriver::IMotorDriver>> m_motorDrivers;
    std::shared_ptr<spdlog::logger> m_logger;
};
}


#endif // MOTORDRIVERMANAGER_H
