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
    , m_sources(basicSourceNames())
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
        if (!QDBusConnection::sessionBus().connect(SOLID_POWERMANAGEMENT_SERVICE,
                                                   QStringLiteral("/org/kde/Solid/PowerManagement/Actions/BrightnessControl"),
                                                   QStringLiteral("org.kde.Solid.PowerManagement.Actions.BrightnessControl"),
                                                   QStringLiteral("brightnessChanged"),
                                                   this,
                                                   SLOT(onScreenBrightness(int)))) {
            qDebug() << "error connecting to Brightness changes via dbus";
        }

        if (!QDBusConnection::sessionBus().connect(SOLID_POWERMANAGEMENT_SERVICE,
                                                   QStringLiteral("/org/kde/Solid/PowerManagement/Actions/BrightnessControl"),
                                                   QStringLiteral("org.kde.Solid.PowerManagement.Actions.BrightnessControl"),
                                                   QStringLiteral("brightnessMaxChanged"),
                                                   this,
                                                   SLOT(onMaximumScreenBrightness(int)))) {
            qDebug() << "error connecting to max brightness changes via dbus";
        }

        if (!QDBusConnection::sessionBus().connect(SOLID_POWERMANAGEMENT_SERVICE,
                                                   QStringLiteral("/org/kde/Solid/PowerManagement/Actions/KeyboardBrightnessControl"),
                                                   QStringLiteral("org.kde.Solid.PowerManagement.Actions.KeyboardBrightnessControl"),
                                                   QStringLiteral("keyboardBrightnessChanged"),
                                                   this,
                                                   SLOT(keyboardBrightnessChanged(int)))) {
            qDebug() << "error connecting to Keyboard Brightness changes via dbus";
        }

        if (!QDBusConnection::sessionBus().connect(SOLID_POWERMANAGEMENT_SERVICE,
                                                   QStringLiteral("/org/kde/Solid/PowerManagement/Actions/KeyboardBrightnessControl"),
                                                   QStringLiteral("org.kde.Solid.PowerManagement.Actions.KeyboardBrightnessControl"),
                                                   QStringLiteral("keyboardBrightnessMaxChanged"),
                                                   this,
                                                   SLOT(maximumKeyboardBrightnessChanged(int)))) {
            qDebug() << "error connecting to max keyboard Brightness changes via dbus";
        }

        if (!QDBusConnection::sessionBus().connect(SOLID_POWERMANAGEMENT_SERVICE,
                                                   QStringLiteral("/org/kde/Solid/PowerManagement/Actions/HandleButtonEvents"),
                                                   QStringLiteral("org.kde.Solid.PowerManagement.Actions.HandleButtonEvents"),
                                                   QStringLiteral("triggersLidActionChanged"),
                                                   this,
                                                   SLOT(triggersLidActionChanged(bool)))) {
            qDebug() << "error connecting to lid action trigger changes via dbus";
        }

        if (!QDBusConnection::sessionBus().connect(SOLID_POWERMANAGEMENT_SERVICE,
                                                   QStringLiteral("/org/kde/Solid/PowerManagement/PolicyAgent"),
                                                   QStringLiteral("org.kde.Solid.PowerManagement.PolicyAgent"),
                                                   QStringLiteral("InhibitionsChanged"),
                                                   this,
                                                   SLOT(inhibitionsChanged(QList<InhibitionInfo>, QStringList)))) {
            qDebug() << "error connecting to inhibition changes via dbus";
        }



        if (!QDBusConnection::sessionBus().connect(SOLID_POWERMANAGEMENT_SERVICE,
                                                   QStringLiteral("/org/kde/Solid/PowerManagement/Actions/PowerProfile"),
                                                   QStringLiteral("org.kde.Solid.PowerManagement.Actions.PowerProfile"),
                                                   QStringLiteral("currentProfileChanged"),
                                                   this,
                                                   SLOT(updatePowerProfileCurrentProfile(QString)))) {
            qDebug() << "error connecting to current profile changes via dbus";
        }

        if (!QDBusConnection::sessionBus().connect(SOLID_POWERMANAGEMENT_SERVICE,
                                                   QStringLiteral("/org/kde/Solid/PowerManagement/Actions/PowerProfile"),
                                                   QStringLiteral("org.kde.Solid.PowerManagement.Actions.PowerProfile"),
                                                   QStringLiteral("profileChoicesChanged"),
                                                   this,
                                                   SLOT(updatePowerProfileChoices(QStringList)))) {
            qDebug() << "error connecting to profile choices changes via dbus";
        }

        if (!QDBusConnection::sessionBus().connect(SOLID_POWERMANAGEMENT_SERVICE,
                                                   QStringLiteral("/org/kde/Solid/PowerManagement/Actions/PowerProfile"),
                                                   QStringLiteral("org.kde.Solid.PowerManagement.Actions.PowerProfile"),
                                                   QStringLiteral("performanceInhibitedReasonChanged"),
                                                   this,
                                                   SLOT(updatePowerProfilePerformanceInhibitedReason(QString)))) {
            qDebug() << "error connecting to inhibition reason changes via dbus";
        }

        if (!QDBusConnection::sessionBus().connect(SOLID_POWERMANAGEMENT_SERVICE,
                                                   QStringLiteral("/org/kde/Solid/PowerManagement/Actions/PowerProfile"),
                                                   QStringLiteral("org.kde.Solid.PowerManagement.Actions.PowerProfile"),
                                                   QStringLiteral("performanceDegradedReasonChanged"),
                                                   this,
                                                   SLOT(updatePowerProfilePerformanceDegradedReason(QString)))) {
            qDebug() << "error connecting to degradation reason changes via dbus";
        }

        if (!QDBusConnection::sessionBus().connect(SOLID_POWERMANAGEMENT_SERVICE,
                                                   QStringLiteral("/org/kde/Solid/PowerManagement/Actions/PowerProfile"),
                                                   QStringLiteral("org.kde.Solid.PowerManagement.Actions.PowerProfile"),
                                                   QStringLiteral("profileHoldsChanged"),
                                                   this,
                                                   SLOT(updatePowerProfileHolds(QList<QVariantMap>)))) {
            qDebug() << "error connecting to profile hold changes via dbus";
        }
    }
}

QStringList PowermanagementEngine::basicSourceNames() const
{
    QStringList sources;
    sources << QStringLiteral("Battery") << QStringLiteral("AC Adapter") << QStringLiteral("Sleep States") << QStringLiteral("PowerDevil")
            << QStringLiteral("Inhibitions") << QStringLiteral("Power Profiles");
    return sources;
}

QStringList PowermanagementEngine::sources() const
{
    return m_sources;
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
    } else if (name == QLatin1String("PowerDevil")) {
        QDBusMessage screenMsg = QDBusMessage::createMethodCall(SOLID_POWERMANAGEMENT_SERVICE,
                                                                QStringLiteral("/org/kde/Solid/PowerManagement/Actions/BrightnessControl"),
                                                                QStringLiteral("org.kde.Solid.PowerManagement.Actions.BrightnessControl"),
                                                                QStringLiteral("brightness"));
        QDBusPendingReply<int> screenReply = QDBusConnection::sessionBus().asyncCall(screenMsg);
        QDBusPendingCallWatcher *screenWatcher = new QDBusPendingCallWatcher(screenReply, this);
        QObject::connect(screenWatcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher *watcher) {
            QDBusPendingReply<int> reply = *watcher;
            if (!reply.isError()) {
                onScreenBrightness(reply.value());
            }
            watcher->deleteLater();
        });

        QDBusMessage maxScreenMsg = QDBusMessage::createMethodCall(SOLID_POWERMANAGEMENT_SERVICE,
                                                                   QStringLiteral("/org/kde/Solid/PowerManagement/Actions/BrightnessControl"),
                                                                   QStringLiteral("org.kde.Solid.PowerManagement.Actions.BrightnessControl"),
                                                                   QStringLiteral("brightnessMax"));
        QDBusPendingReply<int> maxScreenReply = QDBusConnection::sessionBus().asyncCall(maxScreenMsg);
        QDBusPendingCallWatcher *maxScreenWatcher = new QDBusPendingCallWatcher(maxScreenReply, this);
        QObject::connect(maxScreenWatcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher *watcher) {
            QDBusPendingReply<int> reply = *watcher;
            if (!reply.isError()) {
                onMaximumScreenBrightness(reply.value());
            }else
            {
                qDebug() << "POWER" << reply.error().message();
            }
            watcher->deleteLater();
        });

        QDBusMessage keyboardMsg = QDBusMessage::createMethodCall(SOLID_POWERMANAGEMENT_SERVICE,
                                                                  QStringLiteral("/org/kde/Solid/PowerManagement/Actions/KeyboardBrightnessControl"),
                                                                  QStringLiteral("org.kde.Solid.PowerManagement.Actions.KeyboardBrightnessControl"),
                                                                  QStringLiteral("keyboardBrightness"));
        QDBusPendingReply<int> keyboardReply = QDBusConnection::sessionBus().asyncCall(keyboardMsg);
        QDBusPendingCallWatcher *keyboardWatcher = new QDBusPendingCallWatcher(keyboardReply, this);
        QObject::connect(keyboardWatcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher *watcher) {
            QDBusPendingReply<int> reply = *watcher;
            if (!reply.isError()) {
                keyboardBrightnessChanged(reply.value());
            }
            watcher->deleteLater();
        });

        QDBusMessage maxKeyboardMsg = QDBusMessage::createMethodCall(SOLID_POWERMANAGEMENT_SERVICE,
                                                                     QStringLiteral("/org/kde/Solid/PowerManagement/Actions/KeyboardBrightnessControl"),
                                                                     QStringLiteral("org.kde.Solid.PowerManagement.Actions.KeyboardBrightnessControl"),
                                                                     QStringLiteral("keyboardBrightnessMax"));
        QDBusPendingReply<int> maxKeyboardReply = QDBusConnection::sessionBus().asyncCall(maxKeyboardMsg);
        QDBusPendingCallWatcher *maxKeyboardWatcher = new QDBusPendingCallWatcher(maxKeyboardReply, this);
        QObject::connect(maxKeyboardWatcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher *watcher) {
            QDBusPendingReply<int> reply = *watcher;
            if (!reply.isError()) {
                maximumKeyboardBrightnessChanged(reply.value());
            }
            watcher->deleteLater();
        });

        QDBusMessage lidIsPresentMsg = QDBusMessage::createMethodCall(SOLID_POWERMANAGEMENT_SERVICE,
                                                                      QStringLiteral("/org/kde/Solid/PowerManagement"),
                                                                      SOLID_POWERMANAGEMENT_SERVICE,
                                                                      QStringLiteral("isLidPresent"));
        QDBusPendingReply<bool> lidIsPresentReply = QDBusConnection::sessionBus().asyncCall(lidIsPresentMsg);
        QDBusPendingCallWatcher *lidIsPresentWatcher = new QDBusPendingCallWatcher(lidIsPresentReply, this);
        QObject::connect(lidIsPresentWatcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher *watcher) {
            QDBusPendingReply<bool> reply = *watcher;
            if (!reply.isError()) {
                setData(QStringLiteral("PowerDevil"), QStringLiteral("Is Lid Present"), reply.value());
            }
            watcher->deleteLater();
        });

        QDBusMessage triggersLidActionMsg = QDBusMessage::createMethodCall(SOLID_POWERMANAGEMENT_SERVICE,
                                                                           QStringLiteral("/org/kde/Solid/PowerManagement/Actions/HandleButtonEvents"),
                                                                           QStringLiteral("org.kde.Solid.PowerManagement.Actions.HandleButtonEvents"),
                                                                           QStringLiteral("triggersLidAction"));
        QDBusPendingReply<bool> triggersLidActionReply = QDBusConnection::sessionBus().asyncCall(triggersLidActionMsg);
        QDBusPendingCallWatcher *triggersLidActionWatcher = new QDBusPendingCallWatcher(triggersLidActionReply, this);
        QObject::connect(triggersLidActionWatcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher *watcher) {
            QDBusPendingReply<bool> reply = *watcher;
            if (!reply.isError()) {
                triggersLidActionChanged(reply.value());
            }
            watcher->deleteLater();
        });

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
    } else if (name == QLatin1String("Power Profiles")) {
        auto profileMsg = QDBusMessage::createMethodCall(SOLID_POWERMANAGEMENT_SERVICE,
                                                         QStringLiteral("/org/kde/Solid/PowerManagement/Actions/PowerProfile"),
                                                         QStringLiteral("org.kde.Solid.PowerManagement.Actions.PowerProfile"),
                                                         QStringLiteral("currentProfile"));
        auto profileWatcher = new QDBusPendingCallWatcher(QDBusConnection::sessionBus().asyncCall(profileMsg));
        connect(profileWatcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher *watcher) {
            watcher->deleteLater();
            QDBusPendingReply<QString> reply = *watcher;
            if (reply.isError()) {
                return;
            }
            updatePowerProfileCurrentProfile(reply.value());
        });

        auto choicesMsg = QDBusMessage::createMethodCall(SOLID_POWERMANAGEMENT_SERVICE,
                                                         QStringLiteral("/org/kde/Solid/PowerManagement/Actions/PowerProfile"),
                                                         QStringLiteral("org.kde.Solid.PowerManagement.Actions.PowerProfile"),
                                                         QStringLiteral("profileChoices"));
        auto choicesWatcher = new QDBusPendingCallWatcher(QDBusConnection::sessionBus().asyncCall(choicesMsg));
        connect(choicesWatcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher *watcher) {
            watcher->deleteLater();
            QDBusPendingReply<QStringList> reply = *watcher;
            if (reply.isError()) {
                return;
            }
            updatePowerProfileChoices(reply.value());
        });

        auto inhibitedMsg = QDBusMessage::createMethodCall(SOLID_POWERMANAGEMENT_SERVICE,
                                                           QStringLiteral("/org/kde/Solid/PowerManagement/Actions/PowerProfile"),
                                                           QStringLiteral("org.kde.Solid.PowerManagement.Actions.PowerProfile"),
                                                           QStringLiteral("performanceInhibitedReason"));
        auto inhibitedWatcher = new QDBusPendingCallWatcher(QDBusConnection::sessionBus().asyncCall(inhibitedMsg));
        connect(inhibitedWatcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher *watcher) {
            watcher->deleteLater();
            QDBusPendingReply<QString> reply = *watcher;
            if (reply.isError()) {
                return;
            }
            updatePowerProfilePerformanceInhibitedReason(reply.value());
        });

        auto degradedMsg = QDBusMessage::createMethodCall(SOLID_POWERMANAGEMENT_SERVICE,
                                                          QStringLiteral("/org/kde/Solid/PowerManagement/Actions/PowerProfile"),
                                                          QStringLiteral("org.kde.Solid.PowerManagement.Actions.PowerProfile"),
                                                          QStringLiteral("performanceDegradedReason"));
        auto degradedWatcher = new QDBusPendingCallWatcher(QDBusConnection::sessionBus().asyncCall(degradedMsg));
        connect(degradedWatcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher *watcher) {
            watcher->deleteLater();
            QDBusPendingReply<QString> reply = *watcher;
            if (reply.isError()) {
                return;
            };
            updatePowerProfilePerformanceDegradedReason(reply.value());
        });

        auto holdsMsg = QDBusMessage::createMethodCall(SOLID_POWERMANAGEMENT_SERVICE,
                                                       QStringLiteral("/org/kde/Solid/PowerManagement/Actions/PowerProfile"),
                                                       QStringLiteral("org.kde.Solid.PowerManagement.Actions.PowerProfile"),
                                                       QStringLiteral("profileHolds"));
        auto holdsWatcher = new QDBusPendingCallWatcher(QDBusConnection::sessionBus().asyncCall(holdsMsg));
        connect(holdsWatcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher *watcher) {
            watcher->deleteLater();
            QDBusPendingReply<QList<QVariantMap>> reply = *watcher;
            if (reply.isError()) {
                return;
            };
            updatePowerProfileHolds(reply.value());
        });
    } else {
        qDebug() << "Data for '" << name << "' not found";
        return false;
    }
    return true;
}

void PowermanagementEngine::setScreenBrightness(int value, bool silent)
{
    m_job->setScreenBrightness(value, silent);
}


//bool PowermanagementEngine::updateSourceEvent(const QString &source)
//{
//    if (source == QLatin1String("UserActivity")) {
//        setData(QStringLiteral("UserActivity"), QStringLiteral("IdleTime"), KIdleTime::instance()->idleTime());
//        return true;
//    }
//    return Plasma::DataEngine::updateSourceEvent(source);
//}

//Plasma::Service *PowermanagementEngine::serviceForSource(const QString &source)
//{
//    if (source == QLatin1String("PowerDevil")) {
//        return new PowerManagementService(this);
//    }

//    return nullptr;
//}

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

void PowermanagementEngine::keyboardBrightnessChanged(int brightness)
{
    setData(QStringLiteral("PowerDevil"), QStringLiteral("Keyboard Brightness"), brightness);
}

void PowermanagementEngine::maximumKeyboardBrightnessChanged(int maximumBrightness)
{
    setData(QStringLiteral("PowerDevil"), QStringLiteral("Maximum Keyboard Brightness"), maximumBrightness);
    setData(QStringLiteral("PowerDevil"), QStringLiteral("Keyboard Brightness Available"), maximumBrightness > 0);
}

void PowermanagementEngine::triggersLidActionChanged(bool triggers)
{
    setData(QStringLiteral("PowerDevil"), QStringLiteral("Triggers Lid Action"), triggers);
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

void PowermanagementEngine::onMaximumScreenBrightness(int maximumBrightness)
{
    qDebug() << "POWER" << "SCreen maximum brightness" << maximumBrightness;

    emit maximumScreenBrightnessChanged(maximumBrightness);
    emit screenBrightnessAvailableChanged(maximumBrightness > 0);
}

void PowermanagementEngine::onScreenBrightness(int brightness)
{
    qDebug() << "POWER" << "SCreen brightness" << brightness;
    emit screenBrightnessChanged(brightness);
}


#include "powermanagementengine.moc"
