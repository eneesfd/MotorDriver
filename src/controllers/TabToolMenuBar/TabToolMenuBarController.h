#ifndef TABTOOLMENUBARCONTROLLER_H
#define TABTOOLMENUBARCONTROLLER_H

#include <QObject>

namespace spdlog
{
class logger;
}

class TabToolMenuBarController : public QObject
{
    Q_OBJECT
public:
    explicit TabToolMenuBarController(std::shared_ptr<spdlog::logger> logger_,
                                      QObject *parent = nullptr);
private:
    void setupConnections();
    std::shared_ptr<spdlog::logger> m_Logger;
signals:
};

#endif // TABTOOLMENUBARCONTROLLER_H
