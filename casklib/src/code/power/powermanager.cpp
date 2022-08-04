#include "powermanager.h"
#include "login1_manager_interface.h"
#include "upower_interface.h"

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusPendingCallWatcher>
#include <CaskServer/caskpower.h>

PowerManager *PowerManager::m_instance = nullptr;

PowerManager::PowerManager(QObject *parent) : QObject(parent)
//  , m_server(new CaskPower(this))
{
    m_login1 = new OrgFreedesktopLogin1ManagerInterface(QStringLiteral("org.freedesktop.login1"),
                                                        QStringLiteral("/org/freedesktop/login1"),
                                                        QDBusConnection::systemBus(),
                                                        this);

    auto propLoaded = [this](QDBusPendingCallWatcher *watcher, bool *argToUpdate) {
        watcher->deleteLater();
        m_pendingJobs--;
        QDBusPendingReply<QString> reply = *watcher;
        if (reply.isError()) {
            *argToUpdate = false;
        } else {
            // both "yes" and "challenge" will show up in the UI
            const QString value = reply.value();
            *argToUpdate = false;
            if (value == QLatin1String("yes") || value == QLatin1String("challenge")) {
                *argToUpdate = true;
            }
        }

        if (m_pendingJobs == 0)
        {
            //            m_state = SessionManagement::State::Ready;
            //            Q_EMIT stateChanged();
            Q_EMIT canShutdownChanged();
            Q_EMIT canRebootChanged();
            Q_EMIT canSuspendChanged();
            Q_EMIT canHibernateChanged();
        }
    };

    m_pendingJobs = 5;
    {
        auto watcher = new QDBusPendingCallWatcher(m_login1->CanPowerOff(), this);
        connect(watcher, &QDBusPendingCallWatcher::finished, this, std::bind(propLoaded, std::placeholders::_1, &m_canShutdown));
    }
    {
        auto watcher = new QDBusPendingCallWatcher(m_login1->CanReboot(), this);
        connect(watcher, &QDBusPendingCallWatcher::finished, this, std::bind(propLoaded, std::placeholders::_1, &m_canReboot));
    }
    {
        auto watcher = new QDBusPendingCallWatcher(m_login1->CanSuspend(), this);
        connect(watcher, &QDBusPendingCallWatcher::finished, this, std::bind(propLoaded, std::placeholders::_1, &m_canSuspend));
    }
    {
        auto watcher = new QDBusPendingCallWatcher(m_login1->CanHybridSleep(), this);
        connect(watcher, &QDBusPendingCallWatcher::finished, this, std::bind(propLoaded, std::placeholders::_1, &m_canHybridSuspend));
    }
    {
        auto watcher = new QDBusPendingCallWatcher(m_login1->CanHibernate(), this);
        connect(watcher, &QDBusPendingCallWatcher::finished, this, std::bind(propLoaded, std::placeholders::_1, &m_canHibernate));
    }

    connect(m_login1, &OrgFreedesktopLogin1ManagerInterface::PrepareForSleep, this, [this](bool sleeping) {
        if (sleeping) {
            Q_EMIT aboutToSuspend();
        } else {
            Q_EMIT resumingFromSuspend();
        }
    });

//    connect(m_server, &CaskPower::logoutRequested, [this]()
//    {
//       emit this->logoutRequested();
//    });

//    connect(m_server, &CaskPower::shutdownRequested, [this]()
//    {
//       emit this->shutdownRequested();
//    });
}

void PowerManager::shutdown()
{
    logout();
    m_login1->PowerOff(true).waitForFinished();
}

void PowerManager::reboot()
{
    m_login1->Reboot(true).waitForFinished();
}

void PowerManager::suspend()
{
    m_login1->Suspend(true).waitForFinished();
}

void PowerManager::hibernate()
{
    m_login1->Hibernate(true).waitForFinished();
}

void PowerManager::logout()
{
    m_server->logout();
    qApp->quit();
}

bool PowerManager::canShutdown()
{
    return m_canShutdown;
}

bool PowerManager::canReboot()
{
    return m_canReboot;
}

bool PowerManager::canSuspend()
{
    return m_canSuspend;
}

bool PowerManager::canHibernate()
{
    return m_canHibernate;
}
