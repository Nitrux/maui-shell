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

#include <QtQml/QtQml>

#include "notifications.h"
#include "notificationsmodel.h"
#include "historymodel.h"
#include "notificationsimageprovider.h"

class CaskNotificationsPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")
public:
    void initializeEngine(QQmlEngine *engine, const char *uri) override
    {
        engine->addImageProvider(QStringLiteral("notifications"),
                                 new NotificationsImageProvider());
    }

    void registerTypes(const char *uri) override
    {        

        qmlRegisterAnonymousType<NotificationsModel>(uri, 1);
        qmlRegisterAnonymousType<HistoryModel>(uri, 1);
        qmlRegisterType<Notifications>(uri, 1, 0, "Notifications");

    }
};

#include "plugin.moc"
