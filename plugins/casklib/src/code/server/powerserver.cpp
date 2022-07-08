#include "powerserver.h"
#include "poweradaptor.h"

#include <QDBusInterface>

PowerServer::PowerServer(QObject *parent) : QObject(parent)
{
    new PowerAdaptor(this);
    if(!QDBusConnection::sessionBus().registerObject(QStringLiteral("/Power"), this))
    {
        qDebug() << "FAILED TO REGISTER THEME DBUS OBJECT";
        return;
    }
}

void PowerServer::restart()
{
    Q_EMIT restartRequested();
}

void PowerServer::logout()
{
    Q_EMIT logoutRequested();
}

void PowerServer::shutdown()
{
    Q_EMIT shutdownRequested();
}

void PowerServer::sleep()
{
    Q_EMIT sleepRequested();
}
