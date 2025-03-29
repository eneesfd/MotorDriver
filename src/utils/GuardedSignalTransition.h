#ifndef GUARDEDSIGNALTRANSITION_H
#define GUARDEDSIGNALTRANSITION_H

#include <QSignalTransition>
#include <QStateMachine>


class GuardedSignalTransition : public QSignalTransition {
public:

    template<typename Func>
    GuardedSignalTransition(QObject *sender, Func signal, QAbstractState *targetState, QStateMachine* stateMachine_ = nullptr)
        : QSignalTransition(sender, QMetaMethod::fromSignal(signal).methodSignature().constData()),
        m_targetState(targetState),
        m_StateMachine(stateMachine_)
    {
        setTargetState(targetState);
    }

protected:
    bool eventTest(QEvent *e) override {
        if (m_StateMachine && m_StateMachine->configuration().contains(m_targetState))
            return false;
        return QSignalTransition::eventTest(e);
    }
private:
    QAbstractState *m_targetState;
    QStateMachine   *m_StateMachine;
};

#endif // GUARDEDSIGNALTRANSITION_H
