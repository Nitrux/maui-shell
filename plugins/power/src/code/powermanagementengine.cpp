/*
    SPDX-FileCopyrightText: 2007 Aaron Seigo <aseigo@kde.org>
    SPDX-FileCopyrightText: 2007-2008 Sebastian Kuegler <sebas@kde.org>
    SPDX-FileCopyrightText: 2007 Maor Vanmak <mvanmak1@gmail.com>
    SPDX-FileCopyrightText: 2008 Dario Freddi <drf54321@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#include "powermanagementengine.h"

// kde-workspace/libs
#include <sessionmanagement.h>

// solid specific includes
#include <solid/device.h>
#include <solid/deviceinterface.h>
#include <solid/devicenotifier.h>

#include <KAuthorized>
#include <KIdleTime>
#include <klocalizedstring.h>

#include <QDebug>

#include <QDBusConnectionInterface>
#include <QDBusError>
#include <QDBusInterface>
#include <QDBusMetaType>
#include <QDBusPendingCallWatcher>
#include <QDBusReply>

#include "powermanagementservice.h"
#include <Plasma/DataContainer>

static const char SOLID_POWERMANAGEMENT_SERVICE[] = "org.kde.Solid.PowerManagement";

Q_DECLARE_METATYPE(QList<InhibitionInfo>)
Q_DECLARE_METATYPE(InhibitionInfo)

PowermanagementEngine::PowermanagementEngine(QObject *parent, const QVariantList &args)
    : Plasma::DataEngine(parent, args)
    , m_sources(basicSourceNames())
    , m_session(new SessionManagement(this))
{
    Q_UNUSED(args)
    qDBusRegisterMetaType<QList<InhibitionInfo>>();
    qDBusRegisterMetaType<InhibitionInfo>();
    qDBusRegisterMetaType<QList<QVariant>>();
    qDBusRegisterMetaType<QList<QVariantMap>>();
    init();
}

PowermanagementEngine::~PowermanagementEngine()
{
}

void PowermanagementEngine::init()
{
    connect(Solid::DeviceNotifier::instance(), &Solid::DeviceNotifier::deviceAdded, this, &PowermanagementEngine::deviceAdded);
    connect(Solid::DeviceNotifier::instance(), &Solid::DeviceNotifier::deviceRemoved, this, &PowermanagementEngine::deviceRemoved);

    if (QDBusConnection::sessionBus().interface()->isServiceRegistered(SOLID_POWERMANAGEMENT_SERVICE)) {
        if (!QDBusConnection::sessionBus().connect(SOLID_POWERMANAGEMENT_SERVICE,
                                                   QStringLiteral("/org/kde/Solid/PowerManagement/Actions/BrightnessControl"),
                                                   QStringLiteral("org.kde.Solid.PowerManagement.Actions.BrightnessControl"),
                                                   QStringLiteral("brightnessChanged"),
                                                   this,
                                                   SLOT(screenBrightnessChanged(int)))) {
            qDebug() << "error connecting to Brightness changes via dbus";
        }

        if (!QDBusConnection::sessionBus().connect(SOLID_POWERMANAGEMENT_SERVICE,
                                                   QStringLiteral("/org/kde/Solid/PowerManagement/Actions/BrightnessControl"),
                                                   QStringLiteral("org.kde.Solid.PowerManagement.Actions.BrightnessControl"),
                                                   QStringLiteral("brightnessMaxChanged"),
                                                   this,
                                                   SLOT(maximumScreenBrightnessChanged(int)))) {
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
                                                   QStringLiteral("/org/kde/Solid/PowerManagement"),
                                                   SOLID_POWERMANAGEMENT_SERVICE,
                                                   QStringLiteral("batteryRemainingTimeChanged"),
                                                   this,
                                                   SLOT(batteryRemainingTimeChanged(qulonglong)))) {
            qDebug() << "error connecting to remaining time changes";
        }

        if (!QDBusConnection::sessionBus().connect(SOLID_POWERMANAGEMENT_SERVICE,
                                                   QStringLiteral("/org/kde/Solid/PowerManagement"),
                                                   SOLID_POWERMANAGEMENT_SERVICE,
                                                   QStringLiteral("chargeStopThresholdChanged"),
                                                   this,
                                                   SLOT(chargeStopThresholdChanged(int)))) {
            qDebug() << "error connecting to charge stop threshold changes via dbus";
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
    if (name == QLatin1String("Battery")) {
        const QList<Solid::Device> listBattery = Solid::Device::listFromType(Solid::DeviceInterface::Battery);
        m_batterySources.clear();

        if (listBattery.isEmpty()) {
            setData(QStringLiteral("Battery"), QStringLiteral("Has Battery"), false);
            setData(QStringLiteral("Battery"), QStringLiteral("Has Cumulative"), false);
            return true;
        }

        uint index = 0;
        QStringList batterySources;

        foreach (const Solid::Device &deviceBattery, listBattery) {
            const Solid::Battery *battery = deviceBattery.as<Solid::Battery>();

            const QString source = QStringLiteral("Battery%1").arg(index++);

            batterySources << source;
            m_batterySources[deviceBattery.udi()] = source;

            connect(battery, &Solid::Battery::chargeStateChanged, this, &PowermanagementEngine::updateBatteryChargeState);
            connect(battery, &Solid::Battery::chargePercentChanged, this, &PowermanagementEngine::updateBatteryChargePercent);
            connect(battery, &Solid::Battery::energyChanged, this, &PowermanagementEngine::updateBatteryEnergy);
            connect(battery, &Solid::Battery::presentStateChanged, this, &PowermanagementEngine::updateBatteryPresentState);

            // Set initial values
            updateBatteryChargeState(battery->chargeState(), deviceBattery.udi());
            updateBatteryChargePercent(battery->chargePercent(), deviceBattery.udi());
            updateBatteryEnergy(battery->energy(), deviceBattery.udi());
            updateBatteryPresentState(battery->isPresent(), deviceBattery.udi());
            updateBatteryPowerSupplyState(battery->isPowerSupply(), deviceBattery.udi());

            setData(source, QStringLiteral("Vendor"), deviceBattery.vendor());
            setData(source, QStringLiteral("Product"), deviceBattery.product());
            setData(source, QStringLiteral("Capacity"), battery->capacity());
            setData(source, QStringLiteral("Type"), batteryType(battery));
        }

        updateBatteryNames();
        updateOverallBattery();

        setData(QStringLiteral("Battery"), QStringLiteral("Has Battery"), !batterySources.isEmpty());
        if (!batterySources.isEmpty()) {
            setData(QStringLiteral("Battery"), QStringLiteral("Sources"), batterySources);
            QDBusMessage msg = QDBusMessage::createMethodCall(SOLID_POWERMANAGEMENT_SERVICE,
                                                              QStringLiteral("/org/kde/Solid/PowerManagement"),
                                                              SOLID_POWERMANAGEMENT_SERVICE,
                                                              QStringLiteral("batteryRemainingTime"));
            QDBusPendingReply<qulonglong> reply = QDBusConnection::sessionBus().asyncCall(msg);
            QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
            QObject::connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher *watcher) {
                QDBusPendingReply<qulonglong> reply = *watcher;
                if (!reply.isError()) {
                    batteryRemainingTimeChanged(reply.value());
                }
                watcher->deleteLater();
            });
        }

        QDBusMessage msg = QDBusMessage::createMethodCall(SOLID_POWERMANAGEMENT_SERVICE,
                                                          QStringLiteral("/org/kde/Solid/PowerManagement"),
                                                          SOLID_POWERMANAGEMENT_SERVICE,
                                                          QStringLiteral("chargeStopThreshold"));
        QDBusPendingReply<int> reply = QDBusConnection::sessionBus().asyncCall(msg);
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
        QObject::connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher *watcher) {
            QDBusPendingReply<int> reply = *watcher;
            if (!reply.isError()) {
                chargeStopThresholdChanged(reply.value());
            }
            watcher->deleteLater();
        });

        m_sources = basicSourceNames() + batterySources;
    } else if (name == QLatin1String("AC Adapter")) {
        QDBusConnection::sessionBus().connect(QStringLiteral("org.freedesktop.PowerManagement"),
                                              QStringLiteral("/org/freedesktop/PowerManagement"),
                                              QStringLiteral("org.freedesktop.PowerManagement"),
                                              QStringLiteral("PowerSaveStatusChanged"),
                                              this,
                                              SLOT(updateAcPlugState(bool)));

        QDBusMessage msg = QDBusMessage::createMethodCall(QStringLiteral("org.freedesktop.PowerManagement"),
                                                          QStringLiteral("/org/freedesktop/PowerManagement"),
                                                          QStringLiteral("org.freedesktop.PowerManagement"),
                                                          QStringLiteral("GetPowerSaveStatus"));
        QDBusReply<bool> reply = QDBusConnection::sessionBus().call(msg);
        updateAcPlugState(reply.isValid() ? reply.value() : false);
    } else if (name == QLatin1String("Sleep States")) {
        setData(QStringLiteral("Sleep States"), QStringLiteral("Standby"), m_session->canSuspend());
        setData(QStringLiteral("Sleep States"), QStringLiteral("Suspend"), m_session->canSuspend());
        setData(QStringLiteral("Sleep States"), QStringLiteral("Hibernate"), m_session->canHibernate());
        setData(QStringLiteral("Sleep States"), QStringLiteral("HybridSuspend"), m_session->canHybridSuspend());
        setData(QStringLiteral("Sleep States"), QStringLiteral("LockScreen"), m_session->canLock());
        setData(QStringLiteral("Sleep States"), QStringLiteral("Logout"), m_session->canLogout());
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
                screenBrightnessChanged(reply.value());
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
                maximumScreenBrightnessChanged(reply.value());
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
                removeAllData(QStringLiteral("Inhibitions"));

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

QString PowermanagementEngine::batteryType(const Solid::Battery *battery) const
{
    switch (battery->type()) {
    case Solid::Battery::PrimaryBattery:
        return QStringLiteral("Battery");
        break;
    case Solid::Battery::UpsBattery:
        return QStringLiteral("Ups");
        break;
    case Solid::Battery::MonitorBattery:
        return QStringLiteral("Monitor");
        break;
    case Solid::Battery::MouseBattery:
        return QStringLiteral("Mouse");
        break;
    case Solid::Battery::KeyboardBattery:
        return QStringLiteral("Keyboard");
        break;
    case Solid::Battery::PdaBattery:
        return QStringLiteral("Pda");
        break;
    case Solid::Battery::PhoneBattery:
        return QStringLiteral("Phone");
        break;
    case Solid::Battery::GamingInputBattery:
        return QStringLiteral("GamingInput");
        break;
    case Solid::Battery::BluetoothBattery:
        return QStringLiteral("Bluetooth");
        break;
    default:
        return QStringLiteral("Unknown");
    }

    return QStringLiteral("Unknown");
}

bool PowermanagementEngine::updateSourceEvent(const QString &source)
{
    if (source == QLatin1String("UserActivity")) {
        setData(QStringLiteral("UserActivity"), QStringLiteral("IdleTime"), KIdleTime::instance()->idleTime());
        return true;
    }
    return Plasma::DataEngine::updateSourceEvent(source);
}

Plasma::Service *PowermanagementEngine::serviceForSource(const QString &source)
{
    if (source == QLatin1String("PowerDevil")) {
        return new PowerManagementService(this);
    }

    return nullptr;
}

QString PowermanagementEngine::batteryStateToString(int newState) const
{
    QString state(QStringLiteral("Unknown"));
    if (newState == Solid::Battery::NoCharge) {
        state = QLatin1String("NoCharge");
    } else if (newState == Solid::Battery::Charging) {
        state = QLatin1String("Charging");
    } else if (newState == Solid::Battery::Discharging) {
        state = QLatin1String("Discharging");
    } else if (newState == Solid::Battery::FullyCharged) {
        state = QLatin1String("FullyCharged");
    }

    return state;
}

void PowermanagementEngine::updateBatteryChargeState(int newState, const QString &udi)
{
    const QString source = m_batterySources[udi];
    setData(source, QStringLiteral("State"), batteryStateToString(newState));
    updateOverallBattery();
}

void PowermanagementEngine::updateBatteryPresentState(bool newState, const QString &udi)
{
    const QString source = m_batterySources[udi];
    setData(source, QStringLiteral("Plugged in"), newState); // FIXME This needs to be renamed and Battery Monitor adjusted
}

void PowermanagementEngine::updateBatteryChargePercent(int newValue, const QString &udi)
{
    const QString source = m_batterySources[udi];
    setData(source, QStringLiteral("Percent"), newValue);
    updateOverallBattery();
}

void PowermanagementEngine::updateBatteryEnergy(double newValue, const QString &udi)
{
    const QString source = m_batterySources[udi];
    setData(source, QStringLiteral("Energy"), newValue);
}

void PowermanagementEngine::updateBatteryPowerSupplyState(bool newState, const QString &udi)
{
    const QString source = m_batterySources[udi];
    setData(source, QStringLiteral("Is Power Supply"), newState);
}

void PowermanagementEngine::updateBatteryNames()
{
    uint unnamedBatteries = 0;
    foreach (QString source, m_batterySources) {
        DataContainer *batteryDataContainer = containerForSource(source);
        if (batteryDataContainer) {
            const QString batteryVendor = batteryDataContainer->data()[QStringLiteral("Vendor")].toString();
            const QString batteryProduct = batteryDataContainer->data()[QStringLiteral("Product")].toString();

            // Don't show battery name for primary power supply batteries. They usually have cryptic serial number names.
            const bool showBatteryName = batteryDataContainer->data()[QStringLiteral("Type")].toString() != QLatin1String("Battery")
                || !batteryDataContainer->data()[QStringLiteral("Is Power Supply")].toBool();

            if (!batteryProduct.isEmpty() && batteryProduct != QLatin1String("Unknown Battery") && showBatteryName) {
                if (!batteryVendor.isEmpty()) {
                    setData(source, QStringLiteral("Pretty Name"), QString(batteryVendor + ' ' + batteryProduct));
                } else {
                    setData(source, QStringLiteral("Pretty Name"), batteryProduct);
                }
            } else {
                ++unnamedBatteries;
                if (unnamedBatteries > 1) {
                    setData(source, QStringLiteral("Pretty Name"), i18nc("Placeholder is the battery number", "Battery %1", unnamedBatteries));
                } else {
                    setData(source, QStringLiteral("Pretty Name"), i18n("Battery"));
                }
            }
        }
    }
}

void PowermanagementEngine::updateOverallBattery()
{
    const QList<Solid::Device> listBattery = Solid::Device::listFromType(Solid::DeviceInterface::Battery);
    bool hasCumulative = false;

    double energy = 0;
    double totalEnergy = 0;
    bool allFullyCharged = true;
    bool charging = false;
    bool noCharge = false;
    double totalPercentage = 0;
    int count = 0;

    foreach (const Solid::Device &deviceBattery, listBattery) {
        const Solid::Battery *battery = deviceBattery.as<Solid::Battery>();

        if (battery && battery->isPowerSupply()) {
            hasCumulative = true;

            energy += battery->energy();
            totalEnergy += battery->energyFull();
            totalPercentage += battery->chargePercent();
            allFullyCharged = allFullyCharged && (battery->chargeState() == Solid::Battery::FullyCharged);
            charging = charging || (battery->chargeState() == Solid::Battery::Charging);
            noCharge = noCharge || (battery->chargeState() == Solid::Battery::NoCharge);
            ++count;
        }
    }

    if (count == 1) {
        // Energy is sometimes way off causing us to show rubbish; this is a UPower issue
        // but anyway having just one battery and the tooltip showing strange readings
        // compared to the popup doesn't look polished.
        setData(QStringLiteral("Battery"), QStringLiteral("Percent"), totalPercentage);
    } else if (totalEnergy > 0) {
        setData(QStringLiteral("Battery"), QStringLiteral("Percent"), qRound(energy / totalEnergy * 100));
    } else if (count > 0) { // UPS don't have energy, see Bug 348588
        setData(QStringLiteral("Battery"), QStringLiteral("Percent"), qRound(totalPercentage / static_cast<qreal>(count)));
    } else {
        setData(QStringLiteral("Battery"), QStringLiteral("Percent"), 0);
    }

    if (hasCumulative) {
        if (allFullyCharged) {
            setData(QStringLiteral("Battery"), QStringLiteral("State"), "FullyCharged");
        } else if (charging) {
            setData(QStringLiteral("Battery"), QStringLiteral("State"), "Charging");
        } else if (noCharge) {
            setData(QStringLiteral("Battery"), QStringLiteral("State"), "NoCharge");
        } else {
            setData(QStringLiteral("Battery"), QStringLiteral("State"), "Discharging");
        }
    } else {
        setData(QStringLiteral("Battery"), QStringLiteral("State"), "Unknown");
    }

    setData(QStringLiteral("Battery"), QStringLiteral("Has Cumulative"), hasCumulative);
}

void PowermanagementEngine::updateAcPlugState(bool onBattery)
{
    setData(QStringLiteral("AC Adapter"), QStringLiteral("Plugged in"), !onBattery);
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

void PowermanagementEngine::deviceRemoved(const QString &udi)
{
    if (m_batterySources.contains(udi)) {
        Solid::Device device(udi);
        Solid::Battery *battery = device.as<Solid::Battery>();
        if (battery)
            battery->disconnect();

        const QString source = m_batterySources[udi];
        m_batterySources.remove(udi);
        removeSource(source);

        QStringList sourceNames(m_batterySources.values());
        sourceNames.removeAll(source);
        setData(QStringLiteral("Battery"), QStringLiteral("Sources"), sourceNames);
        setData(QStringLiteral("Battery"), QStringLiteral("Has Battery"), !sourceNames.isEmpty());

        updateOverallBattery();
    }
}

void PowermanagementEngine::deviceAdded(const QString &udi)
{
    Solid::Device device(udi);
    if (device.isValid()) {
        const Solid::Battery *battery = device.as<Solid::Battery>();

        if (battery) {
            int index = 0;
            QStringList sourceNames(m_batterySources.values());
            while (sourceNames.contains(QStringLiteral("Battery%1").arg(index))) {
                index++;
            }

            const QString source = QStringLiteral("Battery%1").arg(index);
            sourceNames << source;
            m_batterySources[device.udi()] = source;

            connect(battery, &Solid::Battery::chargeStateChanged, this, &PowermanagementEngine::updateBatteryChargeState);
            connect(battery, &Solid::Battery::chargePercentChanged, this, &PowermanagementEngine::updateBatteryChargePercent);
            connect(battery, &Solid::Battery::energyChanged, this, &PowermanagementEngine::updateBatteryEnergy);
            connect(battery, &Solid::Battery::presentStateChanged, this, &PowermanagementEngine::updateBatteryPresentState);
            connect(battery, &Solid::Battery::powerSupplyStateChanged, this, &PowermanagementEngine::updateBatteryPowerSupplyState);

            // Set initial values
            updateBatteryChargeState(battery->chargeState(), device.udi());
            updateBatteryChargePercent(battery->chargePercent(), device.udi());
            updateBatteryEnergy(battery->energy(), device.udi());
            updateBatteryPresentState(battery->isPresent(), device.udi());
            updateBatteryPowerSupplyState(battery->isPowerSupply(), device.udi());

            setData(source, QStringLiteral("Vendor"), device.vendor());
            setData(source, QStringLiteral("Product"), device.product());
            setData(source, QStringLiteral("Capacity"), battery->capacity());
            setData(source, QStringLiteral("Type"), batteryType(battery));

            setData(QStringLiteral("Battery"), QStringLiteral("Sources"), sourceNames);
            setData(QStringLiteral("Battery"), QStringLiteral("Has Battery"), !sourceNames.isEmpty());

            updateBatteryNames();
            updateOverallBattery();
        }
    }
}

void PowermanagementEngine::batteryRemainingTimeChanged(qulonglong time)
{
    // qDebug() << "Remaining time 2:" << time;
    setData(QStringLiteral("Battery"), QStringLiteral("Remaining msec"), time);
}

void PowermanagementEngine::screenBrightnessChanged(int brightness)
{
    setData(QStringLiteral("PowerDevil"), QStringLiteral("Screen Brightness"), brightness);
}

void PowermanagementEngine::maximumScreenBrightnessChanged(int maximumBrightness)
{
    setData(QStringLiteral("PowerDevil"), QStringLiteral("Maximum Screen Brightness"), maximumBrightness);
    setData(QStringLiteral("PowerDevil"), QStringLiteral("Screen Brightness Available"), maximumBrightness > 0);
}

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
        removeData(QStringLiteral("Inhibitions"), (*it));
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

void PowermanagementEngine::chargeStopThresholdChanged(int threshold)
{
    setData(QStringLiteral("Battery"), QStringLiteral("Charge Stop Threshold"), threshold);
}

K_PLUGIN_CLASS_WITH_JSON(PowermanagementEngine, "plasma-dataengine-powermanagement.json")

#include "powermanagementengine.moc"
