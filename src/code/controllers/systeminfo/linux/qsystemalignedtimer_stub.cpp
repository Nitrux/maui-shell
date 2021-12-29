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
#include "qsystemalignedtimer_stub_p.h"

QTM_BEGIN_NAMESPACE

QSystemAlignedTimerPrivate::QSystemAlignedTimerPrivate(QObject *parent)
    : QObject(parent)
{
}

QSystemAlignedTimerPrivate::~QSystemAlignedTimerPrivate()
{
}

void QSystemAlignedTimerPrivate::wokeUp()
{
}

int QSystemAlignedTimerPrivate::minimumInterval() const
{
    return -1;
}

void QSystemAlignedTimerPrivate::setMinimumInterval(int seconds)
{
    Q_UNUSED(seconds);
}

int QSystemAlignedTimerPrivate::maximumInterval() const
{
    return -1;
}

void QSystemAlignedTimerPrivate::setMaximumInterval(int seconds)
{
    Q_UNUSED(seconds);
}

void QSystemAlignedTimerPrivate::setSingleShot(bool singleShot)
{
    Q_UNUSED(singleShot);
}

bool QSystemAlignedTimerPrivate::isSingleShot() const
{
    return false;
}

void QSystemAlignedTimerPrivate::singleShot(int minimumTime, int maximumTime, QObject *receiver, const char *member)
{
    Q_UNUSED(minimumTime);
    Q_UNUSED(maximumTime);
    Q_UNUSED(receiver);
    Q_UNUSED(member);
}

QSystemAlignedTimer::AlignedTimerError QSystemAlignedTimerPrivate::lastError() const
{
    return QSystemAlignedTimer::AlignedTimerNotSupported;
}

void QSystemAlignedTimerPrivate::start(int minimumTime, int maximumTime)
{
    Q_UNUSED(minimumTime);
    Q_UNUSED(maximumTime);
}

void QSystemAlignedTimerPrivate::start()
{
}

void QSystemAlignedTimerPrivate::stop()
{
}

bool QSystemAlignedTimerPrivate::isActive () const
{
    return false;
}

#include "moc_qsystemalignedtimer_stub_p.cpp"

QTM_END_NAMESPACE
