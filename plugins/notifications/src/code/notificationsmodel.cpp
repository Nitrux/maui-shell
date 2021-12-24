/*
 * SPDX-FileCopyrightText: 2021 Reion Wong <reion@cutefishos.com>
 * SPDX-FileCopyrightText: 2018-2019 Kai Uwe Broulik <kde@privat.broulik.de>
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "notificationsmodel.h"
#include "historymodel.h"
#include "notification.h"

#include <QMetaEnum>
#include <QDebug>

static const int s_notificationsLimit = 1000;

NotificationsModel::NotificationsModel(NotificationServer *server)
    : QAbstractListModel(server)
    ,m_server(server)
    ,m_historyModel(new HistoryModel(this))
{
    m_pendingRemovalTimer.setSingleShot(true);
    m_pendingRemovalTimer.setInterval(50);

    connect(&m_pendingRemovalTimer, &QTimer::timeout, this, [this] {
        QVector<int> rowsToBeRemoved;
        rowsToBeRemoved.reserve(m_pendingRemovals.count());
        for (uint id : qAsConst(m_pendingRemovals)) {
            int row = rowOfNotification(id); // oh the complexity...
            if (row == -1) {
                continue;
            }
            rowsToBeRemoved.append(row);
        }

        removeRows(rowsToBeRemoved);
    });

    connect(m_server, &NotificationServer::notificationAdded, this, &NotificationsModel::onNotificationAdded);
    connect(m_server, &NotificationServer::notificationReplaced, this, &NotificationsModel::onNotificationReplaced);
    connect(m_server, &NotificationServer::notificationRemoved, this, &NotificationsModel::onNotificationRemoved);
}

QVariant NotificationsModel::data(const QModelIndex &index, int role) const
{
    const Notification &notification = m_notifications.at(index.row());

    switch (role) {
    case NotificationsModel::IdRole:
        return notification.id;
    case NotificationsModel::SummaryRole:
        return notification.summary;
    case NotificationsModel::ImageRole:
        return notification.image;
    case NotificationsModel::CreatedRole:
        return notification.created;
    case NotificationsModel::BodyRole:
        return notification.body;
    case NotificationsModel::IconNameRole:
        return notification.appIcon;
    case NotificationsModel::TimeoutRole:
        return notification.timeout;
    case NotificationsModel::AppNameRole:
        return notification.appName;
    case NotificationsModel::PersistentRole:
        return notification.persistent;
    case NotificationsModel::UrgencyRole:
        return notification.urgency;
    case NotificationsModel::CategoryRole:
        return notification.category;
    case NotificationsModel::ActionsRole:
        return notification.actions;
    case NotificationsModel::HasDefaultActionRole:
        return notification.actions.contains("default");
    default:
        break;
    }

    return QVariant();
}

bool NotificationsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    // Notification &notification = m_notifications[index.row()];
    bool dirty = false;
    return dirty;
}

int NotificationsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_notifications.size();
}

QHash<int, QByteArray> NotificationsModel::roleNames() const
{
    static QHash<int, QByteArray> s_roles;

    if (s_roles.isEmpty()) {
        // This generates role names from the Roles enum in the form of: FooRole -> foo
        const QMetaEnum e = QMetaEnum::fromType<NotificationsModel::Roles>();

        // Qt built-in roles we use
        s_roles.insert(Qt::DisplayRole, QByteArrayLiteral("display"));
        s_roles.insert(Qt::DecorationRole, QByteArrayLiteral("decoration"));

        for (int i = 0; i < e.keyCount(); ++i) {
            const int value = e.value(i);

            QByteArray key(e.key(i));
            key[0] = key[0] + 32; // lower case first letter
            key.chop(4); // strip "Role" suffix

            s_roles.insert(value, key);
        }

        s_roles.insert(NotificationsModel::IdRole, QByteArrayLiteral("notificationId")); // id is QML-reserved
    }

    return s_roles;
}

void NotificationsModel::expired(uint id)
{
    int row = rowOfNotification(id);

    if (row > -1) {
        onNotificationRemoved(id, NotificationServer::CloseReason::Expired);
    }
}

void NotificationsModel::close(uint id)
{
    if (rowOfNotification(id) > -1) {
        m_server->closeNotification(id, NotificationServer::CloseReason::DismissedByUser);
    }
}

void NotificationsModel::invokeDefaultAction(uint notificationId)
{
    const int row = rowOfNotification(notificationId);

    if (row == -1) {
        return;
    }

    const Notification &notification = m_notifications.at(row);
    if (!notification.actions.contains("default")) {
        return;
    }

    m_server->InvokeAction(notificationId, "default");
}

int NotificationsModel::rowOfNotification(uint id) const
{
    auto it = std::find_if(m_notifications.constBegin(), m_notifications.constEnd(), [id](const Notification &item) {
        return item.id == id;
    });

    if (it == m_notifications.constEnd()) {
        return -1;
    }

    return std::distance(m_notifications.constBegin(), it);
}

void NotificationsModel::removeRows(const QVector<int> &rows)
{
    if (rows.isEmpty()) {
        return;
    }

    QVector<int> rowsToBeRemoved(rows);
    std::sort(rowsToBeRemoved.begin(), rowsToBeRemoved.end());

    QVector<QPair<int, int>> clearQueue;

    QPair<int, int> clearRange{rowsToBeRemoved.first(), rowsToBeRemoved.first()};

    for (int row : rowsToBeRemoved) {
        if (row > clearRange.second + 1) {
            clearQueue.append(clearRange);
            clearRange.first = row;
        }

        clearRange.second = row;
    }

    if (clearQueue.isEmpty() || clearQueue.last() != clearRange) {
        clearQueue.append(clearRange);
    }

    int rowsRemoved = 0;

    for (int i = clearQueue.count() - 1; i >= 0; --i) {
        const auto &range = clearQueue.at(i);

        beginRemoveRows(QModelIndex(), range.first, range.second);
        for (int j = range.second; j >= range.first; --j) {
            m_notifications.removeAt(j);
            ++rowsRemoved;
        }
        endRemoveRows();
    }

    Q_ASSERT(rowsRemoved == rowsToBeRemoved.count());

    m_pendingRemovals.clear();
}

HistoryModel *NotificationsModel::historyModel() const
{
    return m_historyModel;
}

bool NotificationsModel::doNotDisturb() const
{
    return m_doNotDisturb;
}

void NotificationsModel::onNotificationAdded(const Notification &notification)
{
    // Do Not Disturb Mode:
    // Add directly to the historical model.
    if (m_doNotDisturb)
    {
        m_historyModel->add(notification);
        return;
    }

    if (m_notifications.size() >= s_notificationsLimit) {
        const int cleanupCount = s_notificationsLimit / 2;
        beginRemoveRows(QModelIndex(), 0, cleanupCount - 1);
        for (int i = 0; i < cleanupCount; ++i) {
            m_notifications.removeAt(0);
        }
        endRemoveRows();
    }

    beginInsertRows(QModelIndex(), m_notifications.size(), m_notifications.size());
    m_notifications.append(std::move(notification));
    endInsertRows();
}

void NotificationsModel::onNotificationReplaced(uint replacedId, const Notification &notification)
{
}

void NotificationsModel::onNotificationRemoved(uint removedId, NotificationServer::CloseReason reason)
{
    const int row = rowOfNotification(removedId);

    if (row == -1) {
        return;
    }

    // When a notification expired, keep it around in the history and mark it as such
    if (reason == NotificationServer::CloseReason::Expired) {
        const QModelIndex idx = NotificationsModel::index(row, 0);
        Notification &notification = m_notifications[row];
        // notification.setExpired(true);
        notification.actions.clear();
        emit dataChanged(idx, idx);

        m_historyModel->add(notification);

        // return;
    }

    // Otherwise if explicitly closed by either user or app, mark it for removal
    // some apps are notorious for closing a bunch of notifications at once
    // causing newer notifications to move up and have a dialogs created for them
    // just to then be discarded causing excess CPU usage
    if (!m_pendingRemovals.contains(removedId)) {
        m_pendingRemovals.append(removedId);
    }

    if (!m_pendingRemovalTimer.isActive()) {
        m_pendingRemovalTimer.start();
    }
}

void NotificationsModel::setDoNotDisturb(bool doNotDisturb)
{
    if (m_doNotDisturb == doNotDisturb)
        return;

    m_doNotDisturb = doNotDisturb;
    emit doNotDisturbChanged(m_doNotDisturb);
}
