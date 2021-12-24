/*
 * SPDX-FileCopyrightText: 2021 Reion Wong <reion@cutefishos.com>
 * SPDX-FileCopyrightText: 2018-2019 Kai Uwe Broulik <kde@privat.broulik.de>
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef NOTIFICATIONSMODEL_H
#define NOTIFICATIONSMODEL_H

#include <QAbstractListModel>
#include <QVector>
#include <QTimer>

#include "notificationserver.h"
class HistoryModel;
class Notification;

class NotificationsModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(HistoryModel* historyModel READ historyModel CONSTANT FINAL)
    Q_PROPERTY(bool doNotDisturb READ doNotDisturb WRITE setDoNotDisturb NOTIFY doNotDisturbChanged)

public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        SummaryRole = Qt::DisplayRole,
        ImageRole = Qt::DecorationRole,
        CreatedRole,
        UpdatedRole,
        BodyRole,
        IconNameRole,
        TimeoutRole,
        AppNameRole,
        PersistentRole,
        UrgencyRole,
        CategoryRole,
        ActionsRole,
        HasDefaultActionRole
    };
    Q_ENUM(Roles)

    explicit NotificationsModel(NotificationServer *server );

    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void expired(uint id);

    Q_INVOKABLE void close(uint id);
    Q_INVOKABLE void invokeDefaultAction(uint id);

    int rowOfNotification(uint id) const;
    void removeRows(const QVector<int> &rows);

    HistoryModel* historyModel() const;

    bool doNotDisturb() const;

public slots:
    void setDoNotDisturb(bool doNotDisturb);

signals:
    void doNotDisturbChanged(bool doNotDisturb);

private slots:
    void onNotificationAdded(const Notification &notification);
    void onNotificationReplaced(uint replacedId, const Notification &notification);
    void onNotificationRemoved(uint notificationId, NotificationServer::CloseReason reason);

private:
    NotificationServer *m_server;
    HistoryModel *m_historyModel = nullptr;
    QVector<Notification> m_notifications;
    QVector<uint /*notificationId*/> m_pendingRemovals;
    QTimer m_pendingRemovalTimer;
    bool m_doNotDisturb = false;
};



#endif // NOTIFICATIONSMODEL_H
