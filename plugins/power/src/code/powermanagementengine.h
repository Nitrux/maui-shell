/*
    SPDX-FileCopyrightText: 2007 Aaron Seigo <aseigo@kde.org>
    SPDX-FileCopyrightText: 2007-2008 Sebastian Kuegler <sebas@kde.org>
    SPDX-FileCopyrightText: 2008 Dario Freddi <drf54321@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#pragma once

#include <Plasma/DataEngine>

#include <solid/battery.h>

#include <QDBusConnection>
#include <QHash>
#include <QPair>

class SessionManagement;

using InhibitionInfo = QPair<QString, QString>;

/**
 * This class provides runtime information about the battery and AC status
 * for use in power management Plasma applets.
 */
class PowermanagementEngine : public Plasma::DataEngine
{
    Q_OBJECT

public:
    PowermanagementEngine(QObject *parent, const QVariantList &args);
    ~PowermanagementEngine() override;
    QStringList sources() const override;
    Plasma::Service *serviceForSource(const QString &source) override;

protected:
    bool sourceRequestEvent(const QString &name) override;
    bool updateSourceEvent(const QString &source) override;
    void init();

private Q_SLOTS:
    void updateBatteryChargeState(int newState, const QString &udi);
    void updateBatteryPresentState(bool newState, const QString &udi);
    void updateBatteryChargePercent(int newValue, const QString &udi);
    void updateBatteryEnergy(double newValue, const QString &udi);
    void updateBatteryPowerSupplyState(bool newState, const QString &udi);
    void updateAcPlugState(bool onBattery);
    void updateBatteryNames();
    void updateOverallBattery();

    void deviceRemoved(const QString &udi);
    void deviceAdded(const QString &udi);
    void batteryRemainingTimeChanged(qulonglong time);
    void screenBrightnessChanged(int brightness);
    void maximumScreenBrightnessChanged(int maximumBrightness);
    void keyboardBrightnessChanged(int brightness);
    void maximumKeyboardBrightnessChanged(int maximumBrightness);
    void triggersLidActionChanged(bool triggers);
    void inhibitionsChanged(const QList<InhibitionInfo> &added, const QStringList &removed);
    void chargeStopThresholdChanged(int threshold);

    void updatePowerProfileCurrentProfile(const QString &profile);
    void updatePowerProfileChoices(const QStringList &choices);
    void updatePowerProfilePerformanceInhibitedReason(const QString &reason);
    void updatePowerProfilePerformanceDegradedReason(const QString &reason);
    void updatePowerProfileHolds(const QList<QVariantMap> &holds);

private:
    void populateApplicationData(const QString &name, QString *prettyName, QString *icon);
    QString batteryType(const Solid::Battery *battery) const;
    QStringList basicSourceNames() const;
    QString batteryStateToString(int newState) const;

    QStringList m_sources;

    QHash<QString, QString> m_batterySources; // <udi, Battery0>
    QHash<QString, QPair<QString, QString>> m_applicationInfo; // <appname, <pretty name, icon>>

    SessionManagement *m_session;
};
