#pragma once

#include <QObject>
#include "polkitagentlistener.h"

class Agent : public QObject
{
    Q_OBJECT
    Q_PROPERTY(PolKitAgentListener* listener READ listener CONSTANT FINAL)

public:
    explicit Agent(QObject *parent = nullptr);

    PolKitAgentListener* listener() const;

private:
    PolKitAgentListener *m_listener;

Q_SIGNALS:
    void error();
};
