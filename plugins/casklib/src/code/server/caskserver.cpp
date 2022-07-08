#include "caskserver.h"
#include <QDBusConnectionInterface>
#include <QDebug>

#include "code/server/powerserver.h"

static const QString caskServerName= QStringLiteral("org.maui.Cask");
CaskServer::CaskServer(QObject *parent) : QObject(parent)
  ,m_power(nullptr)
{
        qRegisterMetaType<PowerServer *>("const PowerServer*"); // this is needed for QML to know of FMList in the search method

    if(init())
    {
        m_power = new PowerServer(this);
    }
}

PowerServer *CaskServer::power()
{
    return m_power;
}

bool CaskServer::init()
{
    QDBusConnectionInterface *iface = QDBusConnection::sessionBus().interface();

    if(iface->isServiceRegistered("org.cask.Server"))
    {
        qWarning() << "Service is already registered";
        return true;
    }

    auto registration = iface->registerService(QStringLiteral("org.cask.Server"),
                                               QDBusConnectionInterface::ReplaceExistingService,
                                               QDBusConnectionInterface::DontAllowReplacement);

    if (!registration.isValid())
    {
        qWarning("2 Failed to register D-Bus service \"%s\" on session bus: \"%s\"",
                 qPrintable("org.cask.Server"),
                 qPrintable(registration.error().message()));

        return false;
    }

    return true;
}
