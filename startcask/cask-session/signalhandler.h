/*
    SPDX-FileCopyrightText: 2021 Aleix Pol Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#pragma once

#include <QObject>
#include <QSet>
#include <QSocketNotifier>

/**
 * Class to be able to receive ANSI C signals and forward them onto the Qt eventloop
 *
 * It's a singleton as it relies on static data getting defined.
 */
class SignalHandler : public QObject
{
    Q_OBJECT
public:
    ~SignalHandler() override;
    void addSignal(int signal);

    static SignalHandler *self();

Q_SIGNALS:
    void signalReceived(int signal);

private:
    SignalHandler();
    void handleSignal();
    static void signalHandler(int signal);

    QSet<int> m_signalsRegistered;
    static int signalFd[2];
    QSocketNotifier *m_handler = nullptr;
};
