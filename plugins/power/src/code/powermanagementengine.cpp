/*
    SPDX-FileCopyrightText: 2007 Aaron Seigo <aseigo@kde.org>
    SPDX-FileCopyrightText: 2007-2008 Sebastian Kuegler <sebas@kde.org>
    SPDX-FileCopyrightText: 2007 Maor Vanmak <mvanmak1@gmail.com>
    SPDX-FileCopyrightText: 2008 Dario Freddi <drf54321@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#include "powermanagementengine.h"
#include <solid/battery.h>

// solid specific includes
#include <solid/device.h>
#include <solid/deviceinterface.h>
#include <solid/devicenotifier.h>

#include <KAuthorized>
#include <KIdleTime>
#include <klocalizedstring.h>
#include <KService>
#include <QDebug>

#include <QDBusConnectionInterface>
#include <QDBusError>
#include <QDBusInterface>
#include <QDBusMetaType>
#include <QDBusPendingCallWatcher>
#include <QDBusReply>

#include "powermanagementjob.h"

static const char SOLID_POWERMANAGEMENT_SERVICE[] = "org.kde.Solid.PowerManagement";

Q_DECLARE_METATYPE(QList<InhibitionInfo>)
Q_DECLARE_METATYPE(InhibitionInfo)

PowermanagementEngine::PowermanagementEngine(QObject *parent)
    : QObject(parent)
    , m_job(new PowerManagementJob(this))
{
    qDBusRegisterMetaType<QList<InhibitionInfo>>();
    qDBusRegisterMetaType<InhibitionInfo>();
    qDBusRegisterMetaType<QList<QVariant>>();
    qDBusRegisterMetaType<QList<QVariantMap>>();
    init();
}

void PowermanagementEngine::init()
{
    if (QDBusConnection::sessionBus().interface()->isServiceRegistered(SOLID_POWERMANAGEMENT_SERVICE))
    {
        qDebug() << "POWER IS REGISTERED" << SOLID_POWERMANAGEMENT_SERVICE;



    }
}

bool PowermanagementEngine::sourceRequestEvent(const QString &name)
{
 if (name == QLatin1String("Sleep States")) {
        //        setData(QStringLiteral("Sleep States"), QStringLiteral("Standby"), m_session->canSuspend());
        //        setData(QStringLiteral("Sleep States"), QStringLiteral("Suspend"), m_session->canSuspend());
        //        setData(QStringLiteral("Sleep States"), QStringLiteral("Hibernate"), m_session->canHibernate());
        //        setData(QStringLiteral("Sleep States"), QStringLiteral("HybridSuspend"), m_session->canHybridSuspend());
        //        setData(QStringLiteral("Sleep States"), QStringLiteral("LockScreen"), m_session->canLock());
        //        setData(QStringLiteral("Sleep States"), QStringLiteral("Logout"), m_session->canLogout());
    } else if (name == QLatin1String("Inhibitions")) {
        QDBusMessage inhibitionsMsg = QDBusMessage::createMethodCall(SOLID_POWERMANAGEMENT_SERVICE,
                                                                     QStringLiteral("/org/kde/Solid/PowerManagement/PolicyAgent"),
                                                                     QStringLiteral("org.kde.Solid.PowerManagement.PolicyAgent"),
                                                                     QStringLiteral("ListInhibitions"));
        QDBusPendingReply<QList<InhibitionInfo>> inhibitionsReply = QDBusConnection::sessionBus().asyncCall(inhibitionsMsg);
        QDBusPendingCallWatcher *inhibitionsWatcher = new QDBusPendingCallWatcher(inhibitionsReply, this);
        QObject::connect(inhibitionsWatcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher *watcher) {
            QDBusPendingReply<QList<InhibitionInfo>> reply = *watcher;
            watcher->deleteLater();

            if (!reply.isError()) {
                //                removeAllData(QStringLiteral("Inhibitions"));

                inhibitionsChanged(reply.value(), QStringList());
            }
        });

        // any info concerning lock screen/screensaver goes here
    } else if (name == QLatin1String("UserActivity")) {
        setData(QStringLiteral("UserActivity"), QStringLiteral("IdleTime"), KIdleTime::instance()->idleTime());
    } else {
        qDebug() << "Data for '" << name << "' not found";
        return false;
    }
    return true;
}

void PowermanagementEngine::setData(const QString &name, const QString &name2, const QVariant &value)
{
    qDebug() << "POWER DATA CHANGED" << name << name2 << value;
}

void PowermanagementEngine::inhibitionsChanged(const QList<InhibitionInfo> &added, const QStringList &removed)
{
    for (auto it = removed.constBegin(); it != removed.constEnd(); ++it) {
        //        removeData(QStringLiteral("Inhibitions"), (*it));
    }

    for (auto it = added.constBegin(); it != added.constEnd(); ++it) {
        const QString &name = (*it).first;
        QString prettyName;
        QString icon;
        const QString &reason = (*it).second;

//        populateApplicationData(name, &prettyName, &icon);

        setData(QStringLiteral("Inhibitions"),
                name,
                QVariantMap{{QStringLiteral("Name"), prettyName}, {QStringLiteral("Icon"), icon}, {QStringLiteral("Reason"), reason}});
    }
}



