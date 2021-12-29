/****************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qsystemalignedtimer.h"
#include "qsystemalignedtimer_meego_p.h"
#include <QDebug>

#include <errno.h>
#include <stdio.h>

QTM_BEGIN_NAMESPACE

QSystemAlignedTimerPrivate::QSystemAlignedTimerPrivate(QObject *parent)
    : QObject(parent)
    , m_lastError(QSystemAlignedTimer::NoError)
    , m_minimumInterval(0)
    , m_maximumInterval(0)
    , m_running(false)
    , m_singleShot(false)
    , m_iphbdHandler(0)
    , m_notifier(0)
{
    m_iphbdHandler = iphb_open(0);

    if (!m_iphbdHandler) {
        m_lastError = QSystemAlignedTimer::InternalError;
        qDebug() << "iphb_open error" << m_iphbdHandler<< errno <<strerror(errno);
        return;
    }

    int sockfd = iphb_get_fd(m_iphbdHandler);
    if (!(sockfd > -1)) {
        m_lastError = QSystemAlignedTimer::InternalError;
        qDebug() << "socket failure"<<strerror(errno);
        return;
    }

    m_notifier = new QSocketNotifier(sockfd, QSocketNotifier::Read);
    if (!QObject::connect(m_notifier, SIGNAL(activated(int)), this, SLOT(heartbeatReceived(int)))) {
        delete m_notifier, m_notifier = 0;
        m_lastError = QSystemAlignedTimer::TimerFailed;
        qDebug() << "timer failure";
        return;
    }
    m_notifier->setEnabled(false);
}

QSystemAlignedTimerPrivate::~QSystemAlignedTimerPrivate()
{
    if (m_iphbdHandler)
        (void)iphb_close(m_iphbdHandler);

    if (m_notifier)
        delete m_notifier;
}

void QSystemAlignedTimerPrivate::wokeUp()
{
    if (!m_running)
        return;

    if (!(m_iphbdHandler && m_notifier)) {
        m_lastError = QSystemAlignedTimer::InternalError;
        emit error(m_lastError);
        return;
    }

    m_notifier->setEnabled(false);

    (void)iphb_I_woke_up(m_iphbdHandler);

    m_running = false;
    m_lastError = QSystemAlignedTimer::NoError;

    if (!m_singleShot)
        start();
}

int QSystemAlignedTimerPrivate::minimumInterval() const
{
    return m_minimumInterval;
}

void QSystemAlignedTimerPrivate::setMinimumInterval(int seconds)
{
    m_minimumInterval = seconds;
}

int QSystemAlignedTimerPrivate::maximumInterval() const
{
    return m_maximumInterval;
}

void QSystemAlignedTimerPrivate::setMaximumInterval(int seconds)
{
    m_maximumInterval = seconds;
}

void QSystemAlignedTimerPrivate::setSingleShot(bool singleShot)
{
    m_singleShot = singleShot;
}

bool QSystemAlignedTimerPrivate::isSingleShot() const
{
    return m_singleShot;
}

void QSystemAlignedTimerPrivate::singleShot(int minimumTime, int maximumTime, QObject *receiver, const char *member)
{
    if (receiver && member) {
        QSystemAlignedTimerPrivate *alignedTimer = new QSystemAlignedTimerPrivate(receiver);

        alignedTimer->m_singleShot = true;

        connect(alignedTimer, SIGNAL(timeout()), receiver, member);
        alignedTimer->start(minimumTime, maximumTime);
    }
}

QSystemAlignedTimer::AlignedTimerError QSystemAlignedTimerPrivate::lastError() const
{
    return m_lastError;
}

void QSystemAlignedTimerPrivate::start(int minimumTime, int maximumTime)
{
    m_minimumInterval = minimumTime;
    m_maximumInterval = maximumTime;

    start();
}

void QSystemAlignedTimerPrivate::start()
{
    if (m_running)
        return;

    if (!(m_iphbdHandler && m_notifier)) {
        m_lastError = QSystemAlignedTimer::InternalError;
        emit error(m_lastError);
        return;
    }

    int mustWait = 0;
    time_t unixTime = iphb_wait(m_iphbdHandler, m_minimumInterval, m_maximumInterval, mustWait);

    if (unixTime == (time_t)-1) {
        m_lastError = QSystemAlignedTimer::TimerFailed;
        emit error(m_lastError);
        return;
    }

    m_notifier->setEnabled(true);
    m_running = true;
    m_lastError = QSystemAlignedTimer::NoError;
}

void QSystemAlignedTimerPrivate::stop()
{
    if (!m_running)
        return;

    if (!(m_iphbdHandler && m_notifier)) {
        m_lastError = QSystemAlignedTimer::InternalError;
        emit error(m_lastError);
        return;
    }

    m_notifier->setEnabled(false);

    (void)iphb_discard_wakeups(m_iphbdHandler);

    m_running = false;
    m_lastError = QSystemAlignedTimer::NoError;
}

void QSystemAlignedTimerPrivate::heartbeatReceived(int sock) {
    Q_UNUSED(sock);

    stop();
    emit timeout();

    if (!m_singleShot)
        start();
}

bool QSystemAlignedTimerPrivate::isActive() const
{
    return m_running;
}

#include "moc_qsystemalignedtimer_meego_p.cpp"

QTM_END_NAMESPACE
