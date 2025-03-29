#ifndef PREFERENCESCONTROLLER_H
#define PREFERENCESCONTROLLER_H

#include <QObject>

namespace spdlog
{
class logger;
}

class PreferencesController : public QObject
{
    Q_OBJECT
public:
    explicit PreferencesController(std::shared_ptr<spdlog::logger> logger_,
                                   QObject *parent = nullptr);
    void setupConnections();
private:
    std::shared_ptr<spdlog::logger> m_logger;
signals:
};

#endif // PREFERENCESCONTROLLER_H
