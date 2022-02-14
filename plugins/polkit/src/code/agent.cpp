#include "agent.h"
#include "polkitagentlistener.h"
#include <PolkitQt1/Subject>

Agent::Agent(QObject *parent) : QObject(parent)
  ,m_listener(new PolKitAgentListener(this))
{
    PolkitQt1::UnixSessionSubject session(getpid());

    if (!m_listener->registerListener(session, QStringLiteral("/com/cask/PolicyKit1/AuthenticationAgent")))
        emit error();
}

PolKitAgentListener *Agent::listener() const
{
    return m_listener;
}
