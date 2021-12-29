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

#include "qbatteryinfo.h"

#include <QtCore/qnumeric.h>

#if defined(QT_SIMULATOR)
#  include "simulator/qsysteminfo_simulator_p.h"
#elif defined(Q_OS_LINUX)
#if defined(QT_NO_UPOWER)
#include "linux/qbatteryinfo_linux_p.h"
#else
#include "linux/qbatteryinfo_upower_p.h"
#endif
#elif defined(Q_OS_WIN)
#  include "windows/qbatteryinfo_win_p.h"
#elif defined(Q_OS_MAC)
#  include "mac/qbatteryinfo_mac_p.h"
#else
QT_BEGIN_NAMESPACE
class QBatteryInfoPrivate
{
public:
    QBatteryInfoPrivate(QBatteryInfo *) {}
    QBatteryInfoPrivate(int batteryIndex, QBatteryInfo *): index(batteryIndex) {}

    int batteryCount() { return -1; }
    int batteryIndex() { return index; }
    bool isValid() { return false; }
    void setBatteryIndex(int batteryIndex) { index = batteryIndex; }
    int currentFlow() { return 0; }
    int cycleCount() { return -1; }
    int maximumCapacity() { return -1; }
    int remainingCapacity() { return -1; }
    int remainingChargingTime() { return -1; }
    int voltage() { return -1; }
    QBatteryInfo::ChargerType chargerType() { return QBatteryInfo::UnknownCharger; }
    QBatteryInfo::ChargingState chargingState() { return QBatteryInfo::UnknownChargingState; }
    QBatteryInfo::LevelStatus levelStatus() { return QBatteryInfo::LevelUnknown; }
    QBatteryInfo::Health health() { return QBatteryInfo::HealthUnknown; }
    float temperature() { return qQNaN(); }
private:
    int index;
};
QT_END_NAMESPACE
#endif

#include <QtCore/qmetaobject.h>

QT_BEGIN_NAMESPACE

/*!
    \class QBatteryInfo
    \inmodule QtSystemInfo
    \brief The QBatteryInfo class provides various information about the batteries.
    \ingroup systeminfo

    Note that on some platforms, listening to the signals could lead to a heavy CPU usage. Therefore,
    you are strongly suggested to disconnect the signals when no longer needed in your application.

    Battery index starts at \c 0, which indicates the first battery.
*/

/*!
    \enum QBatteryInfo::ChargerType
    This enum describes the type of charger used.

    \value UnknownCharger           The charger type is unknown, or no charger.
    \value WallCharger              Using wall (mains) charger.
    \value USBCharger               Using USB charger when the system cannot differentiate the current.
    \value VariableCurrentCharger   Using variable current charger such as bicycle or solar.
*/

/*!
    \enum QBatteryInfo::ChargingState
    This enum describes the charging state.

    \value UnknownChargingState  The charging state is unknown or charging error occured.
    \value Charging              The battery is charging.
    \value IdleChargingState     The battery is idle (neither Charging nor Discharging)
    \value Discharging           The battery is discharging.
*/

/*!
    \enum QBatteryInfo::LevelStatus
    This enum describes the level status of the battery.

    \value LevelUnknown              Battery level undetermined.
    \value LevelEmpty                Battery is considered be empty and device needs to shut down.
    \value LevelLow                  Battery level is low and warnings need to be issued to the user.
    \value LevelOk                   Battery level is Ok. It is above "Low" but not "Full".
    \value LevelFull                 Battery is fully charged.
*/

/*!
    \enum QBatteryInfo::Health
    This enum describes the health of the battery.

    \value HealthUnknown    Battery health undetermined
    \value HealthOk         Battery health is OK
    \value HealthBad        Battery health is bad
*/

/*!
    Constructs a \l QBatteryInfo object with the given \a parent. The \l batteryIndex()
    will default to \c 0.
*/
QBatteryInfo::QBatteryInfo(QObject *parent)
    : QObject(parent)
#if !defined(QT_SIMULATOR)
    , d_ptr(new QBatteryInfoPrivate(this))
#else
    , d_ptr(new QBatteryInfoSimulator(this))
#endif // QT_SIMULATOR

{
}

/*!
    Constructs a \l QBatteryInfo object with the given \a batteryIndex and \a parent.
*/
QBatteryInfo::QBatteryInfo(int batteryIndex, QObject *parent)
    : QObject(parent)
#if !defined(QT_SIMULATOR)
    , d_ptr(new QBatteryInfoPrivate(batteryIndex, this))
#else
    , d_ptr(new QBatteryInfoSimulator(batteryIndex, this))
#endif // QT_SIMULATOR

{
}

/*!
    Destroys the object
*/
QBatteryInfo::~QBatteryInfo()
{
}

/*!
    \property QBatteryInfo::batteryCount
    \brief The number of batteries available.

    In case of an error or if the information is not available \c -1 is returned.
*/
int QBatteryInfo::batteryCount() const
{
    return d_ptr->batteryCount();
}

/*!
  \property QBatteryInfo::batteryIndex
  \brief The current battery index

  The first battery is represented by \c 0.
*/
int QBatteryInfo::batteryIndex() const
{
    return d_ptr->batteryIndex();
}

void QBatteryInfo::setBatteryIndex(int batteryIndex)
{
    d_ptr->setBatteryIndex(batteryIndex);
}

/*!
  \property QBatteryInfo::valid
  \brief The validity of this instance

  If this property returns \c false \l batteryIndex() is the only other method that will return
  a valid value. All other methods will return default or invalid values and should not be relied
  upon for displaying to the user or for comparisons.
*/
bool QBatteryInfo::isValid() const
{
    return d_ptr->isValid();
}

/*!
  \property QBatteryInfo::currentFlow
  \brief The current flow of the battery

  This value is measured in milliamperes (mA). A positive returned value means the battery is
  discharging, while a negative value means the battery is charging. In case of an error or if the
  information is not available \c 0 is returned.
*/
int QBatteryInfo::currentFlow() const
{
    return d_ptr->currentFlow();
}

/*!
  \property QBatteryInfo::level
  \brief The level of the battery as a percentage

  In case of an error or if the information is not available \c -1 is returned.

  \sa maximumCapacity(), remainingCapacity(), levelStatus()
*/
int QBatteryInfo::level() const
{
    return d_ptr->level();
}

/*!
  \property QBatteryInfo::cycleCount
  \brief The cycle count of the battery

  In case of an error or if the information is not available \c -1 is returned.
 */
int QBatteryInfo::cycleCount() const
{
    return d_ptr->cycleCount();
}

/*!
  \property QBatteryInfo::maximumCapacity
  \brief The maximum capacity of the battery

  This value is measured in mAh. In case of an error or if the information is not available \c -1
  is returned.

  \sa remainingCapacity(), level(), levelStatus()
*/
int QBatteryInfo::maximumCapacity() const
{
    return d_ptr->maximumCapacity();
}

/*!
  \property QBatteryInfo::remainingCapacity
  \brief The remaining capacity of the battery

  This value is measured in mAh. In case of an error or if the information is not available \c -1
  is returned.

  \sa maximumCapacity(), level(), levelStatus()
*/
int QBatteryInfo::remainingCapacity() const
{
    return d_ptr->remainingCapacity();
}

/*!
  \property QBatteryInfo::remainingChargingTime
  \brief The remaining charging time needed for the battery

  This value is measured in seconds. If the battery is full or not charging \c 0 is returned. In
  case of an error or if the information is not available \c -1 is returned.
*/
int QBatteryInfo::remainingChargingTime() const
{
    return d_ptr->remainingChargingTime();
}

/*!
  \property QBatteryInfo::voltage
  \brief The voltage of the battery

  This value is measured in millivolts (mV). In case of an error or if the information is not
  available \c -1 is returned.
*/
int QBatteryInfo::voltage() const
{
    return d_ptr->voltage();
}

/*!
  \property QBatteryInfo::chargerType
  \brief The type of the charger
*/
QBatteryInfo::ChargerType QBatteryInfo::chargerType() const
{
    return d_ptr->chargerType();
}

/*!
  \property QBatteryInfo::chargingState
  \brief The charging state of the battery
*/
QBatteryInfo::ChargingState QBatteryInfo::chargingState() const
{
    return d_ptr->chargingState();
}

/*!
  \property QBatteryInfo::levelStatus
  \brief The level status of the battery

  This represents an Empty/Low/Full style representation of the \l level().

  \sa maximumCapacity(), remainingCapacity(), level()
*/
QBatteryInfo::LevelStatus QBatteryInfo::levelStatus() const
{
    return d_ptr->levelStatus();
}

/*!
  \property QBatteryInfo::health
  \brief The health of the battery
*/
QBatteryInfo::Health QBatteryInfo::health() const
{
    return d_ptr->health();
}

/*!
  \property QBatteryInfo::temperature
  \brief The temperature of the battery

  This value is measured in Celsius. In case of an error or if the information is not available,
  \c NaN is returned.

  \sa qQNaN()
 */
float QBatteryInfo::temperature() const
{
    return d_ptr->temperature();
}

/*!
    \internal

    Returns the signal that corresponds to \a proxySignal in the
    meta-object of the \a sourceObject.
*/
QMetaMethod proxyToSourceSignal(const QMetaMethod &proxySignal, QObject *sourceObject)
{
    if (!proxySignal.isValid())
        return proxySignal;
    Q_ASSERT(proxySignal.methodType() == QMetaMethod::Signal);
    Q_ASSERT(sourceObject != 0);
    const QMetaObject *sourceMeta = sourceObject->metaObject();
    int sourceIndex = sourceMeta->indexOfSignal(proxySignal.methodSignature());
    Q_ASSERT(sourceIndex != -1);
    return sourceMeta->method(sourceIndex);
}

/*!
    \internal
*/
void QBatteryInfo::connectNotify(const QMetaMethod &signal)
{
#if defined(Q_OS_LINUX) || defined(Q_OS_WIN) || defined(QT_SIMULATOR) || defined(Q_OS_MAC)
    QMetaMethod sourceSignal = proxyToSourceSignal(signal, d_ptr);
    connect(d_ptr, sourceSignal, this, signal, Qt::UniqueConnection);
#else
    Q_UNUSED(signal)
#endif
}

/*!
    \internal
*/
void QBatteryInfo::disconnectNotify(const QMetaMethod &signal)
{
#if defined(Q_OS_LINUX) || defined(Q_OS_WIN) || defined(QT_SIMULATOR) || defined(Q_OS_MAC)
    // We can only disconnect with the private implementation, when there is no receivers for the signal.
    if (isSignalConnected(signal))
        return;

    QMetaMethod sourceSignal = proxyToSourceSignal(signal, d_ptr);
    disconnect(d_ptr, sourceSignal, this, signal);
#else
    Q_UNUSED(signal)
#endif
}

QT_END_NAMESPACE
