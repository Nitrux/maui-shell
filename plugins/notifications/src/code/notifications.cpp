/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:LGPLv3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#include "notifications.h"
#include "notificationsdaemon.h"

Notifications::Notifications(QObject *parent)
    : QObject(parent)
    , m_daemon(new NotificationsDaemon(this))
{
}

bool Notifications::isActive() const
{
    return m_active;
}

void Notifications::invokeAction(uint id, const QString &actionId)
{
    Q_EMIT m_daemon->ActionInvoked(id, actionId);
}

void Notifications::closeNotification(uint id, const CloseReason &reason)
{
    if (m_daemon->m_notifications.remove(id) > 0)
        Q_EMIT m_daemon->NotificationClosed(id, (uint)reason);
}

void Notifications::componentComplete()
{
    // Register service
    if (m_daemon->registerService()) {
        m_active = true;
        Q_EMIT activeChanged();
    }
}
