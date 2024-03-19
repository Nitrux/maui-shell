/*
    SPDX-FileCopyrightText: 2000 Matthias Ettrich <ettrich@kde.org>
    SPDX-FileCopyrightText: 2005 Lubos Lunak <l.lunak@kde.org>
    SPDX-FileCopyrightText: 2018 David Edmundson <davidedmundson@kde.org>

    SPDX-FileContributor: Oswald Buddenhagen <ob6@inf.tu-dresden.de>

    some code taken from the dcopserver (part of the KDE libraries), which is
    SPDX-FileCopyrightText: 1999 Matthias Ettrich <ettrich@kde.org>
    SPDX-FileCopyrightText: 1999 Preston Brown <pbrown@kde.org>

    SPDX-License-Identifier: MIT
*/

#include "startup.h"

#include "debug.h"

#include <unistd.h>

#include <KCompositeJob>
#include <KConfig>
#include <KConfigGroup>
#include <KIO/DesktopExecParser>
#include <KProcess>
#include <KService>

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusServiceWatcher>
#include <QDBusMessage>
#include <QDBusPendingCall>
#include <QDir>
#include <QProcess>
#include <QStandardPaths>
#include <QTimer>
#include <QUrl>

#include "sessiontrack.h"
//#include "startupadaptor.h"
#include <signal.h>

#include "config-startcask.h"
#include "startcask.h"


class Phase : public KCompositeJob
{
    Q_OBJECT
public:
    Phase(const AutoStart &autostart, QObject *parent)
        : KCompositeJob(parent)
        , m_autostart(autostart)
    {
    }

    bool addSubjob(KJob *job) override
    {
        bool rc = KCompositeJob::addSubjob(job);
        job->start();
        return rc;
    }

    void slotResult(KJob *job) override
    {
        KCompositeJob::slotResult(job);
        if (!hasSubjobs()) {
            emitResult();
        }
    }

protected:
    const AutoStart m_autostart;
};

class StartupPhase0 : public Phase
{
    Q_OBJECT
public:
    StartupPhase0(const AutoStart &autostart, QObject *parent)
        : Phase(autostart, parent)
    {
    }
    void start() override
    {
        qCDebug(CASK_SESSION) << "Phase 0";
   //     addSubjob(new AutoStartAppsJob(m_autostart, 0));
//        addSubjob(new KCMInitJob());
     //   addSubjob(new SleepJob());
    }
};

class StartupPhase1 : public Phase
{
    Q_OBJECT
public:
    StartupPhase1(const AutoStart &autostart, QObject *parent)
        : Phase(autostart, parent)
    {
    }
    void start() override
    {
        qCDebug(CASK_SESSION) << "Phase 1";
 //       addSubjob(new AutoStartAppsJob(m_autostart, 1));
    }
};

class StartupPhase2 : public Phase
{
    Q_OBJECT
public:
    StartupPhase2(const AutoStart &autostart, QObject *parent)
        : Phase(autostart, parent)
    {
    }

    void start() override
    {
        qCDebug(CASK_SESSION) << "Phase 2";
        addSubjob(new AutoStartAppsJob(m_autostart, 2));
//        addSubjob(new KDEDInitJob());
    }
};

SleepJob::SleepJob()
{
}

void SleepJob::start()
{
    auto t = new QTimer(this);
    connect(t, &QTimer::timeout, this, [this]() {
        emitResult();
    });
    t->start(100);
}

Startup::Startup(QObject *parent)
    : QObject(parent)
{
    Q_ASSERT(!s_self);
    s_self = this;

    signal(SIGINT, sigHandler);

//    const AutoStart autostart;

    //for wayland
//    {
//        // This must block until started as it sets the WAYLAND_DISPLAY/DISPLAY env variables needed for the rest of the boot
//        // fortunately it's very fast as it's just starting a wrapper
////        StartServiceJob kwinWaylandJob(QStringLiteral("kwin_wayland_wrapper"), {QStringLiteral("--xwayland")}, QStringLiteral("org.kde.KWinWrapper"));
////        kwinWaylandJob.exec();
//        // kslpash is only launched in plasma-session from the wayland mode, for X it's in startplasma-x11

//        const KConfig cfg(QStringLiteral("ksplashrc"));
//        // the splashscreen and progress indicator
//        KConfigGroup ksplashCfg = cfg.group("KSplash");
//        if (ksplashCfg.readEntry("Engine", QStringLiteral("KSplashQML")) == QLatin1String("KSplashQML")) {
//            QProcess::startDetached(QStringLiteral("ksplashqml"), {});
//        }
//    }

    // Keep for KF5; remove in KF6 (KInit will be gone then)
//    QProcess::execute(QStringLiteral(CMAKE_INSTALL_FULL_LIBEXECDIR_KF5 "/start_kdeinit_wrapper"), QStringList());

//    KJob *phase1 = nullptr;
    m_lock.reset(new QEventLoopLocker);

    const QVector<KJob *> sequence = {
//        new StartProcessJob(QStringLiteral("kcminit_startup"), {}),
//        new StartServiceJob(QStringLiteral("kded5"), {}, QStringLiteral("org.kde.kded5"), {}),
//        new StartServiceJob(QStringLiteral("/usr/lib/org_kde_powerdevil"), {},"", {}),

        new StartServiceJob(QStringLiteral("CaskServer"), QStringList(), QStringLiteral("org.cask.Server")),
        new StartServiceJob(QStringLiteral("MauiManServer"), QStringList(), QStringLiteral("org.mauiman.Manager")),

        new StartServiceJob(QStringLiteral("cask"), QStringList(), QStringLiteral("")),

//        new StartupPhase0(autostart, this),
//        phase1 = new StartupPhase1(autostart, this),
//        new RestoreSessionJob(),
//        new StartupPhase2(autostart, this),
    };

    qDebug() << "PASHE 0000000";
    KJob *last = nullptr;
    for (KJob *job : sequence)
    {
        if (!job) {
            continue;
        }
        if (last) {
            connect(last, &KJob::finished, job, &KJob::start);
        }
        last = job;
    }

    connect(sequence.last(), &KJob::finished, this, &Startup::finishStartup);

    qDebug() << "PASHE 0000000 /end";

    sequence.first()->start();

    // app will be closed when all KJobs finish thanks to the QEventLoopLocker in each KJob
}

void Startup::upAndRunning(const QString &msg)
{
//    QDBusMessage ksplashProgressMessage = QDBusMessage::createMethodCall(QStringLiteral("org.kde.KSplash"),
//                                                                         QStringLiteral("/KSplash"),
//                                                                         QStringLiteral("org.kde.KSplash"),
//                                                                         QStringLiteral("setStage"));
//    ksplashProgressMessage.setArguments(QList<QVariant>() << msg);
//    QDBusConnection::sessionBus().asyncCall(ksplashProgressMessage);
}

void Startup::finishStartup()
{
    qDebug() << "PASHE 111111111 /end";

    qCDebug(CASK_SESSION) << "Finished";
//    upAndRunning(QStringLiteral("ready"));


//    startDetached("cask", {});



    playStartupSound(this);
    new SessionTrack(m_processes);
    deleteLater();
}

void Startup::updateLaunchEnv(const QString &key, const QString &value)
{
    qputenv(key.toUtf8().constData(), value.toUtf8().constData());
}

bool Startup::startDetached(const QString &program, const QStringList &args)
{
    QProcess *p = new QProcess();
    p->setProgram(program);
    p->setArguments(args);
    return startDetached(p);
}

bool Startup::startDetached(QProcess *process)
{
    process->start();
    const bool ret = process->waitForStarted();
    if (ret) {
        m_processes << process;
    }
    return ret;
}

Startup *Startup::s_self = nullptr;

RestoreSessionJob::RestoreSessionJob()
{

}

void RestoreSessionJob::start()
{
//    OrgKdeKSMServerInterfaceInterface ksmserverIface(QStringLiteral("org.kde.ksmserver"), QStringLiteral("/KSMServer"), QDBusConnection::sessionBus());
//    auto pending = ksmserverIface.restoreSession();

//    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pending, this);
//    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this]() {
//        emitResult();
//    });
//    connect(watcher, &QDBusPendingCallWatcher::finished, watcher, &QObject::deleteLater);
}

AutoStartAppsJob::AutoStartAppsJob(const AutoStart &autostart, int phase)
    : m_autoStart(autostart)
{
    m_autoStart.setPhase(phase);
}

void AutoStartAppsJob::start()
{
    qCDebug(CASK_SESSION);

    QTimer::singleShot(0, this, [this]() {
        do {
            QString serviceName = m_autoStart.startService();
            if (serviceName.isEmpty()) {
                // Done
                if (!m_autoStart.phaseDone()) {
                    m_autoStart.setPhaseDone();
                }
                emitResult();
                return;
            }
            KService service(serviceName);
            auto arguments = KIO::DesktopExecParser(service, QList<QUrl>()).resultingArguments();
            if (arguments.isEmpty()) {
                qCWarning(CASK_SESSION) << "failed to parse" << serviceName << "for autostart";
                continue;
            }
            qCInfo(CASK_SESSION) << "Starting autostart service " << serviceName << arguments;
            auto program = arguments.takeFirst();
            if (!Startup::self()->startDetached(program, arguments))
                qCWarning(CASK_SESSION) << "could not start" << serviceName << ":" << program << arguments;
        } while (true);
    });
}

StartServiceJob::StartServiceJob(const QString &process, const QStringList &args, const QString &serviceId, const QProcessEnvironment &additionalEnv)
    : KJob()
    , m_process(new QProcess)
    , m_serviceId(serviceId)
    , m_additionalEnv(additionalEnv)
{
    m_process->setProgram(process);
    m_process->setArguments(args);

    auto watcher = new QDBusServiceWatcher(serviceId, QDBusConnection::sessionBus(), QDBusServiceWatcher::WatchForRegistration, this);
    connect(watcher, &QDBusServiceWatcher::serviceRegistered, this, &StartServiceJob::emitResult);
}

void StartServiceJob::start()
{
    auto env = QProcessEnvironment::systemEnvironment();
    env.insert(m_additionalEnv);
    m_process->setProcessEnvironment(env);

    if (!m_serviceId.isEmpty() && QDBusConnection::sessionBus().interface()->isServiceRegistered(m_serviceId))
    {
        qCDebug(CASK_SESSION) << m_process << "already running";
        emitResult();
        return;
    }

    qCDebug(CASK_SESSION) << "Starting " << m_process->program() << m_process->arguments();

    if (!Startup::self()->startDetached(m_process))
    {
        qCWarning(CASK_SESSION) << "error starting process" << m_process->program() << m_process->arguments();
        emitResult();
    }

    if (m_serviceId.isEmpty())
    {
        emitResult();
    }
}

StartProcessJob::StartProcessJob(const QString &process, const QStringList &args, const QProcessEnvironment &additionalEnv)
    : KJob()
    , m_process(new QProcess(this))
{
    m_process->setProgram(process);
    m_process->setArguments(args);
    auto env = QProcessEnvironment::systemEnvironment();
    env.insert(additionalEnv);
    m_process->setProcessEnvironment(env);

    connect(m_process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), [this](int exitCode) {
        qCInfo(CASK_SESSION) << "process job " << m_process->program() << "finished with exit code " << exitCode;
        emitResult();
    });
}

void StartProcessJob::start()
{
    qCDebug(CASK_SESSION) << "Starting " << m_process->program() << m_process->arguments();

    m_process->start();
}

#include "startup.moc"
