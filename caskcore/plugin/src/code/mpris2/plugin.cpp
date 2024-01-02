// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QtQml/QtQml>

#include "mpris2engine.h"
#include "mpris2player.h"

class Mpris2Plugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")
public:
    void registerTypes(const char *uri)
    {
        Q_ASSERT(QLatin1String(uri) == QLatin1String("Liri.Mpris"));

        // @uri Liri.Mpris
        qmlRegisterType<Mpris2Engine>(uri, 1, 0, "Mpris");
        qmlRegisterUncreatableType<Mpris2Player>(uri, 1, 0, "MprisPlayer",
                                                 QStringLiteral("Cannot create MprisPlayer object"));
    }
};

#include "plugin.moc"
