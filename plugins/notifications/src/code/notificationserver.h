/*
 * SPDX-FileCopyrightText: 2021 Reion Wong <reion@cutefishos.com>
 * SPDX-FileCopyrightText: 2018-2019 Kai Uwe Broulik <kde@privat.broulik.de>
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef NOTIFICATIONSERVER_H
#define NOTIFICATIONSERVER_H

#include <QObject>
#include <QDBusConnection>
#include <QDBusServiceWatcher>
#include <QDBusContext>
#include <QSet>
//#include <QAtomicInt>
#include "notification.h"

class NotificationServer : public QObject, protected QDBusContext
{
    Q_OBJECT

public:
    enum class CloseReason {
        Expired = 1, ///< The notification timed out
        DismissedByUser = 2, ///< The user explicitly closed or acknowledged the notification
        Revoked = 3, ///< The notification was revoked by the issuing app because it is no longer relevant
    };
    Q_ENUM(CloseReason)

    explicit NotificationServer(QObject *parent = nullptr);
~NotificationServer();

    bool registerService();
    void unregisterService();

    // DBus
    uint Notify(const QString &app_name,
                uint replaces_id,
                const QString &app_icon,
                const QString &summary,
                const QString &body,
                const QStringList &actions,
                const QVariantMap &hints,
                int timeout);
    void CloseNotification(uint id);
    QStringList GetCapabilities() const;
    QString GetServerInformation(QString &vendor, QString &version, QString &specVersion) const;

    // Inhibitions
    uint Inhibit(const QString &desktop_entry, const QString &reason, const QVariantMap &hints);
    void UnInhibit(uint cookie);
    bool inhibited() const; // property getter

    // Notifition watcher
    void RegisterWatcher();
    void UnRegisterWatcher();

    void InvokeAction(uint id, const QString &actionKey);

    // Self
    void closeNotification(uint id, CloseReason reason);

Q_SIGNALS:
    // DBus
    void NotificationClosed(uint id, uint reason);
    void ActionInvoked(uint id, const QString &actionKey);

    // Self
    void notificationAdded(const Notification &notification);
    void notificationReplaced(uint replacedId, const Notification &notification);
    void notificationRemoved(uint id, CloseReason reason);

private:
    uint m_highestId = -1;
    QAtomicInt *m_idSeed;
    QSet<QString> m_spamApplications;

    Notification m_lastNotification;
    QDBusServiceWatcher *m_notificationWatcher;
};

#endif // NOTIFICATIONSERVER_H
