/*
 * SPDX-FileCopyrightText: 2021 Reion Wong <reion@cutefishos.com>
 * SPDX-FileCopyrightText: 2018-2019 Kai Uwe Broulik <kde@privat.broulik.de>
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "notificationserver.h"
#include "notifications_adaptor.h"
//#include "utils.h"

#include <QDebug>

const QString serviceName(QStringLiteral("org.freedesktop.Notifications"));
const QString servicePath(QStringLiteral("/org/freedesktop/Notifications"));
Q_LOGGING_CATEGORY(NOTIFICATIONS, "cask.notifications")

NotificationServer::NotificationServer(QObject *parent)
    : QObject(parent)
    , m_notificationWatcher(nullptr)
{
    new NotificationsAdaptor(this);

    // Create a seed for notifications identifiers, starting from 1
    m_idSeed = new QAtomicInt(1);

    // List of applications that will send us too many notifications
    // TODO: Add more from configuration
    m_spamApplications << QStringLiteral("Clementine") << QStringLiteral("Spotify");

}

NotificationServer::~NotificationServer()
{
    unregisterService();
    delete m_idSeed;
}

bool NotificationServer::registerService()
{
    QDBusConnectionInterface *iface = QDBusConnection::sessionBus().interface();

    QDBusConnection bus = QDBusConnection::sessionBus();

    auto objectRegistered = QDBusConnection::sessionBus().registerObject(servicePath, this);
    auto registration = iface->registerService(serviceName,
                                               QDBusConnectionInterface::ReplaceExistingService,
                                               QDBusConnectionInterface::DontAllowReplacement);

    if (!objectRegistered) {
        qCWarning(NOTIFICATIONS,
                  "1 Failed to register D-Bus object \"%s\" on session bus: \"%s\"",
                  qPrintable(servicePath),
                  qPrintable(bus.lastError().message()));
        return false;
    }

    if (!registration.isValid()) {
        qCWarning(NOTIFICATIONS,
                  "2 Failed to register D-Bus service \"%s\" on session bus: \"%s\"",
                  qPrintable(serviceName),
                  qPrintable(registration.error().message()));
        return false;
    }

    return true;
}

void NotificationServer::unregisterService()
{
    QDBusConnection bus = QDBusConnection::sessionBus();
    bus.unregisterObject(servicePath);
    bus.unregisterService(serviceName);
}

uint NotificationServer::Notify(const QString &app_name,
                                uint replaces_id,
                                const QString &app_icon,
                                const QString &summary,
                                const QString &body,
                                const QStringList &actions,
                                const QVariantMap &hints,
                                int timeout)
{
    Q_UNUSED(hints);
    qDebug() << "NOTIFY" << summary;
    uint id = 0;

    const bool wasReplaced = replaces_id > 0;

    if (wasReplaced) {
        id = replaces_id;
    } else {
        if (!m_highestId)
            ++m_highestId;

        id = m_highestId;
        ++m_highestId;
    }

    Notification notification;
    notification.id = id;
    notification.created = QDateTime::currentDateTimeUtc();
    notification.service = message().service();
    notification.summary = summary;
    notification.body = body;
    notification.appName = app_name;
    notification.appIcon = app_icon;
    notification.actions = actions;
    notification.timeout = timeout;

    if (notification.appIcon.startsWith("file://"))
        notification.appIcon = notification.appIcon.replace("file://", "");

    uint pid = 0;
    QDBusReply<uint> pidReply = connection().interface()->servicePid(message().service());
    if (pidReply.isValid()) {
        pid = pidReply.value();
    }

    if (pid > 0) {
        // 查找 app name
        if (notification.appIcon.isEmpty()) {
            // const QString processName = Utils::processNameFromPid(pid);
            // notification.appIcon = processName;
        }
    }

    if (m_lastNotification.appName == notification.appName &&
            m_lastNotification.summary == notification.summary &&
            m_lastNotification.body == notification.body &&
            m_lastNotification.created.msecsTo(notification.created) < 1000) {
        return 0;
    }

    m_lastNotification = notification;

    if (wasReplaced) {
        notification.updated = QDateTime::currentDateTimeUtc();
        emit notificationReplaced(replaces_id, notification);
    } else {
        emit notificationAdded(notification);
    }

    return id;
}

void NotificationServer::CloseNotification(uint id)
{
    Q_UNUSED(id);
}

QStringList NotificationServer::GetCapabilities() const
{
    return QStringList{ QStringLiteral("body"),
                QStringLiteral("body-hyperlinks"),
                QStringLiteral("body-markup"),
                QStringLiteral("body-images"),
                QStringLiteral("icon-static"),
                QStringLiteral("actions"),
                QStringLiteral("persistence"),
                QStringLiteral("inline-reply"),
                QStringLiteral("inhibitions") };
}

QString NotificationServer::GetServerInformation(QString &vendor, QString &version, QString &specVersion) const
{
    vendor = "Maui Shell";
    version = "0.5.0";
    specVersion = "1.2";

    return "Cask";
}

uint NotificationServer::Inhibit(const QString &desktop_entry, const QString &reason, const QVariantMap &hints)
{
    return 0;
}

void NotificationServer::UnInhibit(uint cookie)
{
    Q_UNUSED(cookie)
}

bool NotificationServer::inhibited() const
{
    return false;
}

void NotificationServer::RegisterWatcher()
{
    m_notificationWatcher->addWatchedService(message().service());
}

void NotificationServer::UnRegisterWatcher()
{
    m_notificationWatcher->removeWatchedService(message().service());
}

void NotificationServer::InvokeAction(uint id, const QString &actionKey)
{
    Q_EMIT ActionInvoked(id, actionKey);
}

void NotificationServer::closeNotification(uint id, NotificationServer::CloseReason reason)
{
    emit notificationRemoved(id, reason);
}
