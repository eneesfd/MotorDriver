#ifndef SPLASHSCREENSTATE_H
#define SPLASHSCREENSTATE_H

#include <QState>

namespace spdlog {
class logger;
}

class SplashScreenState : public QState
{
    Q_OBJECT
public:
    SplashScreenState(std::shared_ptr<spdlog::logger>, QState* parent = nullptr);
protected:
    void onEntry(QEvent*) override;
    void onExit(QEvent*) override;
signals:
    emit void splashScreenStateEntered();
    emit void splashScreenStateExited();
private:
    std::shared_ptr<spdlog::logger> m_Logger;

};

#endif // SPLASHSCREENSTATE_H
