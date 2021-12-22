// SPDX-FileCopyrightText: 2020 Carl Schwan <carl@carlschwan.eu>
//
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "notifications_plugin.h"

#include <QQmlEngine>
#include <QResource>

#include "notifications.h"
#include "notificationsmodel.h"
#include "historymodel.h"
#include "notificationsimageprovider.h"

void CaskNotificationsPlugin::registerTypes(const char *uri)
{
    qmlRegisterType<Notifications>(uri, 1, 0, "Notifications");



    qmlRegisterAnonymousType<NotificationsModel>(uri, 1);
    qmlRegisterAnonymousType<HistoryModel>(uri, 1);

}


void CaskNotificationsPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_UNUSED(uri);

    /** IMAGE PROVIDERS **/
    engine->addImageProvider(QStringLiteral("notifications"),
                             new NotificationsImageProvider());
}
