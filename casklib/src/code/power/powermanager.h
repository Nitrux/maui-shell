#pragma once
#include <QObject>

class OrgFreedesktopLogin1ManagerInterface;
class OrgFreedesktopUPowerInterface;

class CaskPower;
class PowerManager : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(PowerManager)
    Q_PROPERTY(bool canShutdown READ canShutdown NOTIFY canShutdownChanged)
    Q_PROPERTY(bool canReboot READ canReboot NOTIFY canRebootChanged)
    Q_PROPERTY(bool canSuspend READ canSuspend NOTIFY canSuspendChanged)
    Q_PROPERTY(bool canHibernate READ canHibernate NOTIFY canHibernateChanged)

public:

    enum Operation
    {
        Logout,
        Shutdown,
        Reboot,
        Suspend,
        Hibernate
    };
    Q_ENUM(Operation)

    static PowerManager *instance()
    {
        if(!m_instance)
        {
            m_instance = new PowerManager;
        }

        return m_instance;
    }


    bool canShutdown();
    bool canReboot();
    bool canSuspend();
    bool canHibernate();

public slots:
    void shutdown();
    void reboot();
    void suspend();
    void hibernate();
    void logout(); //closes the session

private:
    explicit PowerManager(QObject *parent = nullptr);
    static PowerManager *m_instance;

    OrgFreedesktopLogin1ManagerInterface *m_login1;
    CaskPower* m_server;
    //    SessionManagement::State m_state = SessionManagement::State::Loading;
    bool m_canShutdown = false;
    bool m_canReboot = false;
    bool m_canSuspend = false;
    bool m_canHybridSuspend = false;
    bool m_canHibernate = false;

    uint m_pendingJobs = 0;

signals:
    void canShutdownChanged();
    void canRebootChanged();
    void canSuspendChanged();
    void canHybridSuspendChanged();
    void canHibernateChanged();

    void aboutToSuspend();
    void resumingFromSuspend();

    void logoutRequested();
    void shutdownRequested();
    void rebootRequested();

};

