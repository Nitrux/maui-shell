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

#ifndef NOTIFICATIONS_H
#define NOTIFICATIONS_H

#include <QObject>
#include <QQmlParserStatus>
#include "notificationsmodel.h"

class NotificationServer;

class Notifications : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)

    Q_PROPERTY(bool active READ isActive NOTIFY activeChanged)
    Q_PROPERTY(NotificationsModel* notificationsModel READ notificationsModel FINAL CONSTANT)

public:

    explicit Notifications(QObject *parent = nullptr);

    bool isActive() const;

    void classBegin() override {}
    void componentComplete() override;

    NotificationsModel* notificationsModel();

Q_SIGNALS:
    void activeChanged();

private:
    bool m_active = false;
    NotificationServer *m_server = nullptr;
    NotificationsModel *m_model = nullptr;
};

#endif // NOTIFICATIONS_H
