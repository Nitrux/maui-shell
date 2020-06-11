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

WaylandProcessLauncher::WaylandProcessLauncher(QObject *parent)
    : QObject(parent)
{
}

WaylandProcessLauncher::~WaylandProcessLauncher()
{
}

void WaylandProcessLauncher::launch(const QString &program)
{
    QProcess *process = new QProcess(this);

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    process->setProcessEnvironment(env);

    connect(process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            process, &QProcess::deleteLater);
    connect(process, &QProcess::errorOccurred, &QProcess::deleteLater);
    connect(process, &QProcess::errorOccurred, this, &WaylandProcessLauncher::onError);
    connect(process, &QProcess::stateChanged, this, &WaylandProcessLauncher::onStateChanged);

    QStringList arguments;
    arguments << "-platform" << "wayland";
    process->start(program, arguments);

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

