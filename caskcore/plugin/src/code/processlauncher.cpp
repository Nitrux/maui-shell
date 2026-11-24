/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt-project.org/legal
**
** This file is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 3 as published by the Free Software Foundation
** and appearing in the file LICENSE included in the packaging
** of this file.
**
** This code is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU Lesser General Public License for more details.
**
****************************************************************************/

#include "processlauncher.h"
#include <QDebug>
#include <KService>
//#include <KRun>
#include <KIO/ApplicationLauncherJob>
#include <QStringList>

WaylandProcessLauncher::WaylandProcessLauncher(QObject *parent)
    : QObject(parent)
{
}

WaylandProcessLauncher::~WaylandProcessLauncher()
{
}

void WaylandProcessLauncher::launchApp(const QString &app)
{
    KService::Ptr service(new KService(app));
    if (!service->isValid())
        return;
    QProcessEnvironment::systemEnvironment().remove(QStringLiteral("QT_IM_MODULE"));
    auto job = KIO::ApplicationLauncherJob(service);
    job.start();
}

void WaylandProcessLauncher::launch(const QString &program)
{
    if(program.isEmpty())
        return;

    const QStringList command = QProcess::splitCommand(program);
    if(command.isEmpty())
        return;

    QProcess process;
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.remove(QStringLiteral("QT_IM_MODULE"));
    process.setProcessEnvironment(env);
    process.setProgram(command.first());
    process.setArguments(command.mid(1));
    process.setStandardInputFile(QProcess::nullDevice());

    if(!process.startDetached())
        qWarning() << "Failed to launch" << program;
}

void WaylandProcessLauncher::onError(QProcess::ProcessError error)
{
    QProcess *proc = qobject_cast<QProcess *>(sender());
    qWarning() << error << "from" << proc;
    if (proc)
        qWarning() << proc->readAllStandardError();
}

void WaylandProcessLauncher::onStateChanged(QProcess::ProcessState state)
{
    QProcess *proc = qobject_cast<QProcess *>(sender());
    qDebug() << proc << "state changed" << state;
    if (proc)
        qDebug() << proc->readAllStandardError();
}

