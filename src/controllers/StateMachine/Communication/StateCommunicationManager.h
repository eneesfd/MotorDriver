#ifndef STATECOMMUNICATIONMANAGER_H
#define STATECOMMUNICATIONMANAGER_H

#include <QObject>
#include <QSharedPointer>
#include "UIStateMachineContext.h"
#include "BusinessStateMachineContext.h"

class StateCommunicationManager : public QObject
{
    Q_OBJECT
public:
    explicit StateCommunicationManager(
        QSharedPointer<UIStateMachineContext> uiContext,
        QSharedPointer<BusinessStateMachineContext> businessContext,
        QObject *parent = nullptr)
        : QObject(parent)
        , m_UIContext(uiContext)
        , m_BusinessContext(businessContext)
    {
        setupConnections();
    }

private:
    QSharedPointer<UIStateMachineContext> m_UIContext;
    QSharedPointer<BusinessStateMachineContext> m_BusinessContext;

    void setupConnections()
    {
        connect(m_UIContext.data(), &UIStateMachineContext::navigateToSystemConfiguration,
                m_BusinessContext.data(), &BusinessStateMachineContext::navigateToSystemConfiguration);

        // Sürücü kurulumu ve hareket kontrolü
        connect(m_UIContext.data(), &UIStateMachineContext::navigateToDriveSetupAndMotion,
                m_BusinessContext.data(), &BusinessStateMachineContext::navigateToDriveSetupAndMotion);

        // Hızlı ayarlama
        connect(m_UIContext.data(), &UIStateMachineContext::navigateToQuickTuning,
                m_BusinessContext.data(), &BusinessStateMachineContext::navigateToQuickTuning);

        // Uzman ayarlaması
        connect(m_UIContext.data(), &UIStateMachineContext::navigateToExpertTuning,
                m_BusinessContext.data(), &BusinessStateMachineContext::navigateToExpertTuning);

        // Tek eksen hareket kontrolü
        connect(m_UIContext.data(), &UIStateMachineContext::navigateToMotionSingleAxis,
                m_BusinessContext.data(), &BusinessStateMachineContext::navigateToMotionSingleAxis);

        // Bağlantı durumu sinyalleri
        connect(m_UIContext.data(), &UIStateMachineContext::connectionEstablished,
                m_BusinessContext.data(), &BusinessStateMachineContext::connectionEstablished);

        connect(m_UIContext.data(), &UIStateMachineContext::connectionLost,
                m_BusinessContext.data(), &BusinessStateMachineContext::connectionLost);

        // State machine sinyalleri
        connect(m_UIContext.data(), &UIStateMachineContext::driveSetupAndMotionStateEntered,
                m_BusinessContext.data(), &BusinessStateMachineContext::driveSetupAndMotionStateEntered);
        connect(m_UIContext.data(), &UIStateMachineContext::driveSetupAndMotionStateExited,
                m_BusinessContext.data(), &BusinessStateMachineContext::driveSetupAndMotionStateExited);
    }
};

#endif // STATECOMMUNICATIONMANAGER_H
