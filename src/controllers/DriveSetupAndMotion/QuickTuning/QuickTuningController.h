#ifndef QUICKTUNINGCONTROLLER_H
#define QUICKTUNINGCONTROLLER_H

#include <QObject>

namespace spdlog
{
    class logger;
}

class QuickTuningController : public QObject
{
    Q_OBJECT
public:
    explicit QuickTuningController(std::shared_ptr<spdlog::logger> logger_, QObject *parent = nullptr);
private:
    std::shared_ptr<spdlog::logger> m_logger;
signals:
};

#endif // QUICKTUNINGCONTROLLER_H
