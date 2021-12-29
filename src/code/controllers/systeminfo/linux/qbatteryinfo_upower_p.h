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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#ifndef QBATTERYINFO_UPOWER_P_H
#define QBATTERYINFO_UPOWER_P_H

#include <qbatteryinfo.h>

#include <QtCore/qmap.h>
#include <QtCore/QVariantMap>
#include <QtCore/QMap>
#include <QtDBus/QDBusServiceWatcher>
#include "qdevicekitservice_linux_p.h"

QT_BEGIN_NAMESPACE

class QBatteryInfoPrivate : public QObject
{
    Q_OBJECT

public:
    QBatteryInfoPrivate(QBatteryInfo *parent);
    QBatteryInfoPrivate(int batteryIndex, QBatteryInfo *parent);
    ~QBatteryInfoPrivate();

    int batteryCount();
    int batteryIndex() const;
    bool isValid();
    int level(int battery);
    int level();
    int currentFlow(int battery);
    int currentFlow();
    int cycleCount(int battery);
    int cycleCount();
    int maximumCapacity(int battery);
    int maximumCapacity();
    int remainingCapacity(int battery);
    int remainingCapacity();
    int remainingChargingTime(int battery);
    int remainingChargingTime();
    int voltage(int battery);
    int voltage();
    QBatteryInfo::ChargerType chargerType();
    QBatteryInfo::ChargingState chargingState(int battery);
    QBatteryInfo::ChargingState chargingState();
    QBatteryInfo::LevelStatus levelStatus(int battery);
    QBatteryInfo::LevelStatus levelStatus();
    QBatteryInfo::Health health(int battery);
    QBatteryInfo::Health health();
    float temperature(int battery);
    float temperature();

    void setBatteryIndex(int batteryIndex);

Q_SIGNALS:
    void batteryCountChanged(int count);
    int batteryIndexChanged(int batteryIndex);
    void validChanged(bool isValid);
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
    QMap <int,QVariantMap> batteryMap;
    QBatteryInfo::ChargerType cType;
    QBatteryInfo::ChargingState cState;
    QList <QBatteryInfo::Health> healthList;
private Q_SLOTS:
    void uPowerBatteryPropertyChanged(const QString & prop, const QVariant &v);
    void getBatteryStats();
    void deviceAdded(const QString &path);
    void deviceRemoved(const QString &path);
    void connectToUpower();
    void disconnectFromUpower();

private:
    QBatteryInfo * const q_ptr;
    Q_DECLARE_PUBLIC(QBatteryInfo)
    QDBusServiceWatcher *watcher;
    int index;

    void initialize();
    QBatteryInfo::ChargingState getCurrentChargingState(int);
    QBatteryInfo::ChargerType getChargerType(const QString &path);
};

QT_END_NAMESPACE

#endif // QBATTERYINFO_UPOWER_P_H
