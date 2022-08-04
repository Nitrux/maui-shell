/*
    SPDX-FileCopyrightText: 2021 Aleix Pol Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#include "sessiontrack.h"
#include "signalhandler.h"
#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusServiceWatcher>
#include <signal.h>
#include <QDebug>

SessionTrack::SessionTrack(const QVector<QProcess *> &processes)
    : m_processes(processes)
{
    SignalHandler::self()->addSignal(SIGTERM);
//    SignalHandler::self()->addSignal(SIGINT);
    connect(SignalHandler::self(), &SignalHandler::signalReceived, QCoreApplication::instance(), [](int signal)
    {
        if (signal == SIGTERM) {
            QCoreApplication::instance()->exit(0);
        }
    });

    for (auto process : std::as_const(m_processes))
    {
        connect(process,  static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, [this] {
            m_processes.removeAll(static_cast<QProcess *>(sender()));
        });
    }

    QObject::connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, this, &SessionTrack::deleteLater);
}

SessionTrack::~SessionTrack()
{
    qDebug() << "(2) REQUEST TO QUIT SESSION SUB PROCESSES";

    disconnect(this, nullptr, QCoreApplication::instance(), nullptr);


    for (auto p : std::as_const(m_processes))
    {
        qDebug() << "(2) SUB PROCESSES" << p->program();
    }

    for (auto process : std::as_const(m_processes))
    {
        qDebug() << "(2) REQUEST TO TERMINATE SESSION SUB PROCESSES" << process->program();

        process->terminate();
    }

    for (auto p : std::as_const(m_processes))
    {
        qDebug() << "(3) SUB PROCESSES" << p->program();
    }


    for (auto process : std::as_const(m_processes))
    {

        if (process->state() == QProcess::Running && !process->waitForFinished(500))
        {
            qDebug() << "(2) REQUEST TO KILL SESSION SUB PROCESSES" << process->program();

            process->kill();
        } else
        {
            qDebug() << "(2) DELETE PROCESSES" << process->program();

//            delete process;
        }
    }

    for (auto p : std::as_const(m_processes))
    {
        qDebug() << "(4) SUB PROCESSES" << p->program() ;
    }
}
