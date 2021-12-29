/****************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd and/or its subsidiary(-ies).
** Copyright (C) 2018 BlackBerry Limited. All rights reserved.
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtSystems module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QBATTERYINFO_H
#define QBATTERYINFO_H

#include "qsysteminfoglobal.h"
#include <QtCore/qobject.h>

QT_BEGIN_NAMESPACE

#if !defined(QT_SIMULATOR)
class QBatteryInfoPrivate;
#else
class QBatteryInfoSimulator;
#endif // QT_SIMULATOR

class Q_SYSTEMINFO_EXPORT QBatteryInfo : public QObject
{
    Q_OBJECT

    Q_ENUMS(ChargerType)
    Q_ENUMS(ChargingState)
    Q_ENUMS(LevelStatus)
    Q_ENUMS(Health)

    Q_PROPERTY(int batteryCount READ batteryCount NOTIFY batteryCountChanged)
    Q_PROPERTY(int batteryIndex READ batteryIndex WRITE setBatteryIndex NOTIFY batteryIndexChanged)
    Q_PROPERTY(bool valid READ isValid NOTIFY validChanged)
    Q_PROPERTY(int level READ level NOTIFY levelChanged)
    Q_PROPERTY(int currentFlow READ currentFlow NOTIFY currentFlowChanged)
    Q_PROPERTY(int cycleCount READ cycleCount NOTIFY cycleCountChanged)
    Q_PROPERTY(int maximumCapacity READ maximumCapacity)
    Q_PROPERTY(int remainingCapacity READ remainingCapacity NOTIFY remainingCapacityChanged)
    Q_PROPERTY(int remainingChargingTime READ remainingChargingTime NOTIFY remainingChargingTimeChanged)
    Q_PROPERTY(int voltage READ voltage NOTIFY voltageChanged)
    Q_PROPERTY(ChargingState chargingState READ chargingState NOTIFY chargingStateChanged)
    Q_PROPERTY(ChargerType chargerType READ chargerType NOTIFY chargerTypeChanged)
    Q_PROPERTY(LevelStatus levelStatus READ levelStatus NOTIFY levelStatusChanged)
    Q_PROPERTY(Health health READ health NOTIFY healthChanged)
    Q_PROPERTY(float temperature READ temperature NOTIFY temperatureChanged)

public:
    enum ChargerType {
        UnknownCharger = 0,
        WallCharger,
        USBCharger,
        VariableCurrentCharger
    };

    enum ChargingState {
        UnknownChargingState = 0,
        Charging,
        IdleChargingState,
        Discharging
    };

    enum LevelStatus {
        LevelUnknown = 0,
        LevelEmpty,
        LevelLow,
        LevelOk,
        LevelFull
    };

    enum Health {
        HealthUnknown = 0,
        HealthOk,
        HealthBad
    };

    explicit QBatteryInfo(QObject *parent = Q_NULLPTR);
    explicit QBatteryInfo(int batteryIndex, QObject *parent = Q_NULLPTR);
    virtual ~QBatteryInfo();

    int batteryCount() const;
    int batteryIndex() const;
    bool isValid() const;
    int level() const;
    int currentFlow() const;
    int cycleCount() const;
    int maximumCapacity() const;
    int remainingCapacity() const;
    int remainingChargingTime() const;
    int voltage() const;
    QBatteryInfo::ChargingState chargingState() const;
    QBatteryInfo::ChargerType chargerType() const;
    QBatteryInfo::LevelStatus levelStatus() const;
    QBatteryInfo::Health health() const;
    float temperature() const;

    void setBatteryIndex(int batteryIndex);

Q_SIGNALS:
    void batteryIndexChanged(int batteryIndex);
    void validChanged(bool isValid);
    void batteryCountChanged(int count);
    void chargerTypeChanged(QBatteryInfo::ChargerType type);
    void chargingStateChanged(QBatteryInfo::ChargingState state);
    void levelChanged(int level);
    void currentFlowChanged(int flow);
    void cycleCountChanged(int cycleCount);
    void remainingCapacityChanged(int capacity);
    void remainingChargingTimeChanged(int seconds);
    void voltageChanged(int voltage);
    void levelStatusChanged(QBatteryInfo::LevelStatus levelStatus);
    void healthChanged(QBatteryInfo::Health health);
    void temperatureChanged(float temperature);

protected:
    void connectNotify(const QMetaMethod &signal);
    void disconnectNotify(const QMetaMethod &signal);

private:
    Q_DISABLE_COPY(QBatteryInfo)
#if !defined(QT_SIMULATOR)
    QBatteryInfoPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(QBatteryInfo)
#else
    QBatteryInfoSimulator * const d_ptr;
#endif // QT_SIMULATOR
};

QT_END_NAMESPACE

#endif // QBATTERYINFO_H
