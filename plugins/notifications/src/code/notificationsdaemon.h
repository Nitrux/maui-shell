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

#ifndef NOTIFICATIONSDAEMON_H
#define NOTIFICATIONSDAEMON_H

#include <QtCore/QObject>
#include <QtCore/QSet>
#include <QtCore/QLoggingCategory>

class QAtomicInt;
class Notifications;

Q_DECLARE_LOGGING_CATEGORY(NOTIFICATIONS)

class NotificationsDaemon : public QObject
{
    Q_OBJECT
public:
    explicit NotificationsDaemon(Notifications *parent);
    ~NotificationsDaemon();

    bool registerService();
    void unregisterService();

public Q_SLOTS:
    uint Notify(const QString &appName, uint replacesId, const QString &appIcon,
                const QString &summary, const QString &body, const QStringList &actions,
                const QVariantMap &hints, int timeout);

    void CloseNotification(uint id);

    QStringList GetCapabilities();

    QString GetServerInformation(QString &vendor, QString &version, QString &specVersion);

Q_SIGNALS:
    void NotificationClosed(uint id, uint reason);
    void ActionInvoked(uint id, const QString &actionKey);

private:
    Notifications *m_parent;
    QAtomicInt *m_idSeed;
    bool m_valid;
    bool m_active;
    QSet<QString> m_spamApplications;
    QHash<QString, uint> m_replaceableNotifications;
    QHash<uint, QString> m_notifications;

    uint nextId();

    friend class Notifications;
};

#endif // NOTIFICATIONSDAEMON_H
