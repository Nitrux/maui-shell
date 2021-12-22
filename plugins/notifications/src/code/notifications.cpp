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
#include "notificationserver.h"

Notifications::Notifications(QObject *parent)
    : QObject(parent)
    , m_server(new NotificationServer(this))
    , m_model(new NotificationsModel(m_server))
{
}

bool Notifications::isActive() const
{
    return m_active;
}


void Notifications::componentComplete()
{
    // Register service
    if (m_server->registerService()) {
        m_active = true;
        Q_EMIT activeChanged();
    }
}

NotificationsModel *Notifications::notificationsModel()
{
    return m_model;
}
