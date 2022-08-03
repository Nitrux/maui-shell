// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIRI_MPRIS2ENGINE_H
#define LIRI_MPRIS2ENGINE_H

#include <QtCore/QObject>
#include <QtCore/QLoggingCategory>
#include <QtQml/QQmlListProperty>

class QDBusServiceWatcher;
class Mpris2Player;
class PlayersModel;

Q_DECLARE_LOGGING_CATEGORY(MPRIS2)

class Mpris2Engine : public QObject
{
    Q_OBJECT
    Q_PROPERTY(PlayersModel* players READ players FINAL CONSTANT)

public:
    Mpris2Engine(QObject *parent = 0);
    ~Mpris2Engine();

    PlayersModel* players();

private:
    QDBusServiceWatcher *m_watcher;
    PlayersModel * m_model;
};

#endif // LIRI_MPRIS2ENGINE_H
