#ifndef AGENT_H
#define AGENT_H

#include <QObject>

class PolKitAgentListener;
class Agent : public QObject
{
    Q_OBJECT
    Q_PROPERTY(PolKitAgentListener* listener READ listener CONSTANT FINAL)

public:
    explicit Agent(QObject *parent = nullptr);

    PolKitAgentListener* listener() const;

private:
    PolKitAgentListener *m_listener;

signals:
    void error();

};

#endif // AGENT_H
