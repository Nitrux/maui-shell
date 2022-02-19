/*
    SPDX-FileCopyrightText: 2018 David Edmundson <davidedmundson@kde.org>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#pragma once

#include <QEventLoopLocker>
#include <QProcess>

class SessionTrack : public QObject
{
    Q_OBJECT
public:
    SessionTrack(const QVector<QProcess *> &processes);
    ~SessionTrack() override;

private:
    QVector<QProcess *> m_processes;
    QEventLoopLocker m_lock;
};
