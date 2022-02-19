/*
    ksmserver - the KDE session management server

    SPDX-FileCopyrightText: 2018 David Edmundson <davidedmundson@kde.org>

    SPDX-License-Identifier: MIT
*/

#pragma once

#include <KJob>
#include <QEventLoopLocker>
#include <QObject>
#include <QProcessEnvironment>

#include "autostart.h"

class Startup : public QObject
{
    Q_OBJECT
public:
    Startup(QObject *parent);
    void upAndRunning(const QString &msg);
    void finishStartup();

    static Startup *self()
    {
        Q_ASSERT(s_self);
        return s_self;
    }

    bool startDetached(const QString &program, const QStringList &args);
    bool startDetached(QProcess *process);

public Q_SLOTS:
    // alternatively we could drop this and have a rule that we /always/ launch everything through klauncher
    // need resolution from frameworks discussion on kdeinit
    void updateLaunchEnv(const QString &key, const QString &value);

private:
    void autoStart(int phase);

    QVector<QProcess *> m_processes;
    QScopedPointer<QEventLoopLocker> m_lock;
    static Startup *s_self;
};

class SleepJob : public KJob
{
    Q_OBJECT
public:
    SleepJob();
    void start() override;
};

class KCMInitJob : public KJob
{
    Q_OBJECT
public:
    KCMInitJob();
    void start() override;
};

class KDEDInitJob : public KJob
{
    Q_OBJECT
public:
    KDEDInitJob();
    void start() override;
};

class AutoStartAppsJob : public KJob
{
    Q_OBJECT
public:
    AutoStartAppsJob(const AutoStart &autoStart, int phase);
    void start() override;

private:
    AutoStart m_autoStart;
};

/**
 * Launches a process, and waits for the process to start
 */
class StartProcessJob : public KJob
{
    Q_OBJECT
public:
    StartProcessJob(const QString &process, const QStringList &args, const QProcessEnvironment &additionalEnv = QProcessEnvironment());
    void start() override;

private:
    QProcess *m_process;
};

/**
 * Launches a process, and waits for the service to appear on the session bus
 */
class StartServiceJob : public KJob
{
    Q_OBJECT
public:
    StartServiceJob(const QString &process,
                    const QStringList &args,
                    const QString &serviceId,
                    const QProcessEnvironment &additionalEnv = QProcessEnvironment());
    void start() override;

private:
    QProcess *m_process;
    const QString m_serviceId;
    const QProcessEnvironment m_additionalEnv;
};

class RestoreSessionJob : public KJob
{
    Q_OBJECT
public:
    RestoreSessionJob();
    void start() override;

private:
};
