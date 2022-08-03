#pragma once
#include <QObject>

class OrgFreedesktopLogin1ManagerInterface;
class OrgFreedesktopUPowerInterface;

class PowerManager : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(PowerManager)
    Q_PROPERTY(bool canShutdown READ canShutdown NOTIFY canShutdownChanged)

public:

    static PowerManager *instance()
    {
        if(!m_instance)
        {
            m_instance = new PowerManager;
        }

        return m_instance;
    }


    bool canShutdown();
    bool canRestart();
    bool canSleep();
    bool canHibernate();

public slots:
    void shutdown();
    void restart();
    void sleep();
    void hibernate();
    void logout();

private:
    explicit PowerManager(QObject *parent = nullptr);
    static PowerManager *m_instance;

    OrgFreedesktopLogin1ManagerInterface *m_login1;
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

};

