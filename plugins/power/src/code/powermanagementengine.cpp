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

void PowermanagementEngine::updatePowerProfileCurrentProfile(const QString &activeProfile)
{
    setData(QStringLiteral("Power Profiles"), QStringLiteral("Current Profile"), activeProfile);
}

void PowermanagementEngine::updatePowerProfileChoices(const QStringList &choices)
{
    setData(QStringLiteral("Power Profiles"), QStringLiteral("Profiles"), choices);
}

void PowermanagementEngine::updatePowerProfilePerformanceInhibitedReason(const QString &reason)
{
    setData(QStringLiteral("Power Profiles"), QStringLiteral("Performance Inhibited Reason"), reason);
}

void PowermanagementEngine::updatePowerProfilePerformanceDegradedReason(const QString &reason)
{
    setData(QStringLiteral("Power Profiles"), QStringLiteral("Performance Degraded Reason"), reason);
}

void PowermanagementEngine::updatePowerProfileHolds(const QList<QVariantMap> &holds)
{
    QList<QVariantMap> out;
    std::transform(holds.cbegin(), holds.cend(), std::back_inserter(out), [this](const QVariantMap &hold) {
        QString prettyName;
        QString icon;
        populateApplicationData(hold[QStringLiteral("ApplicationId")].toString(), &prettyName, &icon);
        return QVariantMap{
            {QStringLiteral("Name"), prettyName},
            {QStringLiteral("Icon"), icon},
            {QStringLiteral("Reason"), hold[QStringLiteral("Reason")]},
            {QStringLiteral("Profile"), hold[QStringLiteral("Profile")]},
        };
    });
    setData(QStringLiteral("Power Profiles"), QStringLiteral("Profile Holds"), QVariant::fromValue(out));
}



//void PowermanagementEngine::screenBrightnessChanged(int brightness)
//{
//    setData(QStringLiteral("PowerDevil"), QStringLiteral("Screen Brightness"), brightness);
//}

//void PowermanagementEngine::maximumScreenBrightnessChanged(int maximumBrightness)
//{
//    setData(QStringLiteral("PowerDevil"), QStringLiteral("Maximum Screen Brightness"), maximumBrightness);
//    setData(QStringLiteral("PowerDevil"), QStringLiteral("Screen Brightness Available"), maximumBrightness > 0);

//}

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

        populateApplicationData(name, &prettyName, &icon);

        setData(QStringLiteral("Inhibitions"),
                name,
                QVariantMap{{QStringLiteral("Name"), prettyName}, {QStringLiteral("Icon"), icon}, {QStringLiteral("Reason"), reason}});
    }
}

void PowermanagementEngine::populateApplicationData(const QString &name, QString *prettyName, QString *icon)
{
    if (m_applicationInfo.contains(name)) {
        const auto &info = m_applicationInfo.value(name);
        *prettyName = info.first;
        *icon = info.second;
    } else {
        KService::Ptr service = KService::serviceByStorageId(name + ".desktop");
        if (service) {
            *prettyName = service->property(QStringLiteral("Name"), QVariant::Invalid).toString(); // cannot be null
            *icon = service->icon();

            m_applicationInfo.insert(name, qMakePair(*prettyName, *icon));
        } else {
            *prettyName = name;
            *icon = name.section(QLatin1Char('/'), -1).toLower();
        }
    }
}

