#include "powerprofile.h"
#include <QDBusConnectionInterface>
#include <QDBusError>
#include <QDBusInterface>
#include <QDBusMetaType>
#include <QDBusPendingCallWatcher>
#include <QDBusReply>

static const char SOLID_POWERMANAGEMENT_SERVICE[] = "org.kde.Solid.PowerManagement";

PowerProfile::PowerProfile(QObject *parent) : QObject(parent)
{

}

