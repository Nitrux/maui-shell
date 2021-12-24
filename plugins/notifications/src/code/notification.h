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

#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QObject>
#include <QDateTime>
#include <QMetaType>

class Notification
{
public:
    uint id = 0;
    QString service;
    QString summary;
    QString body;
    QString appName;
    QString appIcon;
    QString image;
    QStringList actions;
    int timeout = -1;
    bool persistent = false;
    int urgency = -1;
    QString category;
    QDateTime created;
    QDateTime updated;

    inline bool operator==(const Notification &other) const {
        return service == other.service &&
                summary == other.summary &&
                body == other.body &&
                appName == other.appName &&
                appIcon == other.appIcon &&
                created == other.created &&
                updated == other.updated;
    }
    friend QDataStream &operator<<(QDataStream &argument, const Notification &info);
    friend const QDataStream &operator>>(QDataStream &argument, Notification &info);
};

Q_DECLARE_METATYPE(Notification)

#endif // NOTIFICATION_H
