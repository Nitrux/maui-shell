/*
    SPDX-FileCopyrightText: 2021 Aleix Pol Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#include "signalhandler.h"
#include "debug.h"

#include <QDebug>
#include <signal.h>
#include <sys/socket.h>
#include <unistd.h>

int SignalHandler::signalFd[2];

SignalHandler::SignalHandler()
{
    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, signalFd)) {
        qCWarning(PLASMA_SESSION) << "Couldn't create a socketpair";
        return;
    }

    m_handler = new QSocketNotifier(signalFd[1], QSocketNotifier::Read, this);
    connect(m_handler, &QSocketNotifier::activated, this, &SignalHandler::handleSignal);
}

SignalHandler::~SignalHandler()
{
    for (int sig : std::as_const(m_signalsRegistered)) {
        signal(sig, nullptr);
    }
    close(signalFd[0]);
    close(signalFd[1]);
}

void SignalHandler::addSignal(int signalToTrack)
{
    m_signalsRegistered.insert(signalToTrack);
    signal(signalToTrack, signalHandler);
}

void SignalHandler::signalHandler(int signal)
{
    ::write(signalFd[0], &signal, sizeof(signal));
}

void SignalHandler::handleSignal()
{
    m_handler->setEnabled(false);
    int signal;
    ::read(signalFd[1], &signal, sizeof(signal));
    m_handler->setEnabled(true);

    Q_EMIT signalReceived(signal);
}

SignalHandler *SignalHandler::self()
{
    static SignalHandler s_self;
    return &s_self;
}
