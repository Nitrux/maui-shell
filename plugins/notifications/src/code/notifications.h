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

class NotificationsDaemon;

class Notifications : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_PROPERTY(bool active READ isActive NOTIFY activeChanged)
    Q_INTERFACES(QQmlParserStatus)
public:
    enum CloseReason {
        CloseReasonExpired = 1,
        CloseReasonByUser,
        CloseReasonByApplication
    };
    Q_ENUM(CloseReason)

    explicit Notifications(QObject *parent = nullptr);

    bool isActive() const;

    Q_INVOKABLE void invokeAction(uint id, const QString &actionId);

    Q_INVOKABLE void closeNotification(uint id, const Notifications::CloseReason &reason);

    void classBegin() override {}
    void componentComplete() override;

Q_SIGNALS:
    void activeChanged();
    void notificationReceived(uint notificationId, const QString &appName,
                              const QString &appIcon, bool hasIcon,
                              const QString &summary, const QString &body,
                              const QVariantList &actions, bool isPersistent,
                              int expireTimeout, const QVariantMap &hints);
    void notificationClosed(uint notificationId, uint reason);
    void actionInvoked(uint notificationId, const QString &actionKey);

private:
    bool m_active = false;
    NotificationsDaemon *m_daemon = nullptr;
};

#endif // NOTIFICATIONS_H
