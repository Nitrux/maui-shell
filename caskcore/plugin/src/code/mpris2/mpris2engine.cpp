// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusConnectionInterface>
#include <QtDBus/QDBusServiceWatcher>

#include "mpris2engine.h"
#include "mpris2player.h"
#include "playersmodel.h"

const QString mprisInterface(QStringLiteral("org.mpris.MediaPlayer2"));
const QString mprisPlayerInterface(QStringLiteral("org.mpris.MediaPlayer2.Player"));
const QString mprisPrefix(QStringLiteral("org.mpris.MediaPlayer2."));

Q_LOGGING_CATEGORY(MPRIS2, "liri.mpris2")

Mpris2Engine::Mpris2Engine(QObject *parent)
    : QObject(parent)
    , m_model(new PlayersModel(this))
{
    const QDBusConnection bus = QDBusConnection::sessionBus();
    const auto registeredServices = bus.interface()->registeredServiceNames();

    if (registeredServices.isValid()) {
        const auto services = registeredServices.value().filter(mprisInterface);
        for (const auto &name : services) {
            qDebug() << "Found player" << name;
            m_model->append(new Mpris2Player(name));
        }
    }

    m_watcher = new QDBusServiceWatcher(QStringLiteral("org.mpris.MediaPlayer2*"), QDBusConnection::sessionBus(), QDBusServiceWatcher::WatchForOwnerChange, this);
    qDebug() << m_watcher->watchedServices();
//    connect(m_watcher, &QDBusServiceWatcher::serviceRegistered, [this](QString serviceName)
//    {
//        qDebug() << "Found new player meh" << serviceName;

//    });

    connect(m_watcher, &QDBusServiceWatcher::serviceOwnerChanged, [=](const QString &name, const QString &oldOwner, const QString &newOwner) {
        qDebug() << "Found new player meh" << name;

        if (oldOwner.isEmpty() && name.startsWith(mprisPrefix)) {
            qDebug() << "Found new player" << name;
            m_model->append(new Mpris2Player(name));
        } else if (newOwner.isEmpty() && name.startsWith(mprisPrefix)) {

            m_model->remove(m_model->indexOf(name));
        }
    });

    qDebug() << m_watcher->watchedServices();

}

Mpris2Engine::~Mpris2Engine()
{

}

PlayersModel *Mpris2Engine::players()
{
    return m_model;
}



#include "moc_mpris2engine.cpp"
