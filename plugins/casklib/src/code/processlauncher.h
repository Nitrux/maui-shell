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

#ifndef PROCESSLAUNCHER_H
#define PROCESSLAUNCHER_H

#include <QObject>
#include <QProcess>

class WaylandProcessLauncher : public QObject
{
    Q_OBJECT

public:
    explicit WaylandProcessLauncher(QObject *parent = 0);
    ~WaylandProcessLauncher();
    Q_INVOKABLE void launch(const QString &program);
    Q_INVOKABLE void launchApp(const QString &program);
protected slots:
    void onError(QProcess::ProcessError error);
    void onStateChanged(QProcess::ProcessState state);
};

#endif // PROCESSLAUNCHER_H
