/****************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd and/or its subsidiary(-ies).
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

#include "qudevwrapper_p.h"

#include <QMetaMethod>
#include <QSocketNotifier>

#include <poll.h>
#include <sys/select.h>
#if !defined(QT_NO_UDEV)

QT_BEGIN_NAMESPACE

QUDevWrapper::QUDevWrapper(QObject *parent)
    : QObject(parent)
    , udev(0)
    , udevMonitor(0)
    , udevFd(-1)
    , notifier(0)
    , watcherEnabled(false)
    , watchPowerSupply(false)
    , watchDrives(false)
{
}

bool QUDevWrapper::addUDevWatcher(const QByteArray &subsystem)
{
    if (!udev)
        udev = udev_new();

    if (udev && !udevMonitor)
        udevMonitor = udev_monitor_new_from_netlink(udev, "udev");

    if (udevMonitor) {
        if (udev_monitor_filter_add_match_subsystem_devtype(udevMonitor, subsystem, NULL) >= 0) {
            if (!watcherEnabled) {
                if (udev_monitor_enable_receiving(udevMonitor) >= 0) {
                    udevFd = udev_monitor_get_fd(udevMonitor);
                    if (udevFd >= 0) {
                        notifier = new QSocketNotifier(udevFd, QSocketNotifier::Read, this);
                        if (connect(notifier, SIGNAL(activated(int)), this, SLOT(onUDevChanges()))) {
                            watcherEnabled = true;
                            return true;
                        }
                    }
                }
            } else {
                if (udev_monitor_filter_update(udevMonitor) >= 0)
                    return true;
            }
        }
    }
    return false;
}

bool QUDevWrapper::removeAllUDevWatcher()
{
    if (!udev || !udevMonitor)
        return true;

    if (udev_monitor_filter_remove(udevMonitor) >= 0) {
        if (udev_monitor_filter_update(udevMonitor) >= 0)
            return true;
    }
    return false;
}

void QUDevWrapper::connectNotify(const QMetaMethod &signal)
{
    static const QMetaMethod driveChangedSignal = QMetaMethod::fromSignal(&QUDevWrapper::driveChanged);
    static const QMetaMethod batteryDataChangedSignal = QMetaMethod::fromSignal(&QUDevWrapper::batteryDataChanged);
    static const QMetaMethod chargerTypeChangedSignal = QMetaMethod::fromSignal(&QUDevWrapper::chargerTypeChanged);

    if (!watchDrives && signal == driveChangedSignal) {
        if (addUDevWatcher("block"))
            watchDrives = true;
    } else if (!watchPowerSupply && (signal == batteryDataChangedSignal
                                     || signal == chargerTypeChangedSignal)) {
        if (addUDevWatcher("power_supply"))
            watchPowerSupply = true;
    }
}
void QUDevWrapper::disconnectNotify(const QMetaMethod &signal)
{
    static const QMetaMethod driveChangedSignal = QMetaMethod::fromSignal(&QUDevWrapper::driveChanged);
    static const QMetaMethod batteryDataChangedSignal = QMetaMethod::fromSignal(&QUDevWrapper::batteryDataChanged);
    static const QMetaMethod chargerTypeChangedSignal = QMetaMethod::fromSignal(&QUDevWrapper::chargerTypeChanged);

    if (watchDrives && signal == driveChangedSignal) {
        if (removeAllUDevWatcher()) {
            watchDrives = false;
            if (watchPowerSupply) {
                if (!addUDevWatcher("power_supply"))
                    watchPowerSupply = false;
            }
        }
    } else if (watchPowerSupply && (signal == batteryDataChangedSignal
                                    || signal == chargerTypeChangedSignal)) {
        if (removeAllUDevWatcher()) {
            watchPowerSupply = false;
            if (watchDrives) {
                if (!addUDevWatcher("block"))
                    watchDrives = false;
            }
        }
    }

    if (!watchDrives && !watchPowerSupply) {
        disconnect(notifier, SIGNAL(activated(int)), this, SLOT(onUDevChanges()));
        udev_monitor_unref(udevMonitor);
        udevMonitor = 0;
        udevFd = -1;
        watcherEnabled = false;
        udev_unref(udev);
    }
}

void QUDevWrapper::onUDevChanges()
{
    int ret;
    struct udev_device *device = 0;
    QByteArray subsystem;
    QByteArray action;
    QByteArray sysname;
    struct pollfd pollfds[1];

    pollfds[0].fd = udevFd;
    pollfds[0].events = POLLIN;
    pollfds[0].revents = 0;
    ret = poll(pollfds, 1, -1);

    if ((pollfds[0].revents & POLLIN) && ret == 1) {
        device = udev_monitor_receive_device(udevMonitor);
        if (device) {
            subsystem = udev_device_get_subsystem(device);
            action = udev_device_get_action(device);
            sysname = udev_device_get_sysname(device);

#if defined(QT_SIMULATOR)
            if (qstrcmp(subsystem, "block") == 0 && qstrcmp(action, "change") == 0)
                emit driveChanged();
#endif
            if (qstrcmp(subsystem, "block") == 0
                    && ((qstrcmp(action, "add") == 0) || qstrcmp(action, "remove") == 0)) {
                emit driveChanged();
            } else if (qstrcmp(subsystem, "power_supply") == 0) {
                int i = -1;
                if (sysname.contains("AC")) {
                    bool enabled = false;
                    if (qstrcmp(udev_device_get_sysattr_value(device, "online"), "1") == 0)
                        enabled = true;
                    emit chargerTypeChanged("AC", enabled);
                } else if (sysname.contains("USB")) {
                    bool enabled = false;
                    QByteArray charger(udev_device_get_sysattr_value(device, "type"));
                    if (qstrcmp(udev_device_get_sysattr_value(device, "present"), "1") == 0)
                        enabled = true;
                    emit chargerTypeChanged(charger, enabled);
                } else if (sysname.contains("BAT")) {
                    bool ok;
                    i = sysname.right(1).toInt(&ok);
                    if (!ok)
                        i = -1;
                }

                if (i > -1) {
                    QByteArray status(udev_device_get_sysattr_value(device, "status"));
                    if (!status.isEmpty())
                        emit batteryDataChanged(i, "status", status);

                    QByteArray remainingCapacity(udev_device_get_sysattr_value(device, "charge_now"));
                    if (!remainingCapacity.isEmpty())
                        emit batteryDataChanged(i, "charge_now", remainingCapacity);

                    QByteArray remainingChargingTime(udev_device_get_sysattr_value(device, "time_to_full_avg"));
                    if (!remainingChargingTime.isEmpty())
                        emit batteryDataChanged(i, "time_to_full_avg", remainingChargingTime);

                    QByteArray voltage(udev_device_get_sysattr_value(device, "voltage_now"));
                    if (!voltage.isEmpty())
                        emit batteryDataChanged(i, "voltage_now", voltage);

                    QByteArray currentFlow(udev_device_get_sysattr_value(device, "current_now"));
                    if (!currentFlow.isEmpty())
                        emit batteryDataChanged(i, "current_now", currentFlow);

                    QByteArray batteryStatus(udev_device_get_sysattr_value(device, "capacity_level"));
                    if (!batteryStatus.isEmpty())
                        emit batteryDataChanged(i, "capacity_level", batteryStatus);
                }
            }
            udev_device_unref(device);
        }
    }
}

QT_END_NAMESPACE

#endif // QT_NO_UDEV
