/*
 * Copyright (C) 2021 CutefishOS Team.
 *
 * Author:     Reion Wong <reion@cutefishos.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "historymodel.h"
#include "datehelper.h"

#include <QSettings>
#include <QDataStream>
#include <QMetaEnum>

HistoryModel::HistoryModel(QObject *parent)
    : QAbstractListModel(parent)
{
    initDatas();
}

QVariant HistoryModel::data(const QModelIndex &index, int role) const
{
    const Notification &notification = m_notifications.at(index.row());

    switch (role) {
    case HistoryModel::IdRole:
        return notification.id;
    case HistoryModel::SummaryRole:
        return notification.summary;
    case HistoryModel::ImageRole:
        return notification.image;
    case HistoryModel::CreatedRole:
        return DateHelper::friendlyTime(notification.created);
    case HistoryModel::UpdatedRole:
        return DateHelper::friendlyTime(notification.updated);
    case HistoryModel::BodyRole:
        return notification.body;
    case HistoryModel::IconNameRole:
        return notification.appIcon;
    case HistoryModel::TimeoutRole:
        return notification.timeout;
    case HistoryModel::AppNameRole:
        return notification.appName;
    case HistoryModel::PersistentRole:
        return notification.persistent;
    case HistoryModel::UrgencyRole:
        return notification.urgency;
    case HistoryModel::CategoryRole:
        return notification.category;
    case HistoryModel::HasDefaultActionRole:
        return notification.actions.contains("default");
    default:
        break;
    }

    return QVariant();
}

int HistoryModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_notifications.size();
}

QHash<int, QByteArray> HistoryModel::roleNames() const
{
    static QHash<int, QByteArray> s_roles;

    if (s_roles.isEmpty()) {
        // This generates role names from the Roles enum in the form of: FooRole -> foo
        const QMetaEnum e = QMetaEnum::fromType<HistoryModel::Roles>();

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

        s_roles.insert(HistoryModel::IdRole, QByteArrayLiteral("notificationId")); // id is QML-reserved
    }

    return s_roles;
}

void HistoryModel::add(const Notification &notification)
{
    beginInsertRows(QModelIndex(), 0, 0);
    m_notifications.prepend(std::move(notification));
    endInsertRows();
    save();
}

void HistoryModel::remove(int index)
{
    if (index > m_notifications.size() && index < 0)
        return;

    beginRemoveRows(QModelIndex(), index, index);
    m_notifications.removeAt(index);
    endRemoveRows();
    save();
}

void HistoryModel::clearAll()
{
    beginResetModel();
    m_notifications.clear();
    endResetModel();
    save();
}

void HistoryModel::save()
{
    QSettings settings(QSettings::UserScope, "cutefishos", "notifications");
    settings.clear();

    QByteArray datas;
    QDataStream out(&datas, QIODevice::WriteOnly);

    out << m_notifications;

    settings.setValue("datas", datas);
}

void HistoryModel::initDatas()
{
    QSettings settings(QSettings::UserScope, "cutefishos", "notifications");
    QByteArray listByteArray = settings.value("datas").toByteArray();
    QDataStream in(&listByteArray, QIODevice::ReadOnly);
    in >> m_notifications;
}

void HistoryModel::updateTime()
{
    emit layoutChanged();
}
