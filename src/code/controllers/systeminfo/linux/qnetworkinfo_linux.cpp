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

#include "qnetworkinfo_linux_p.h"

#if !defined(QT_NO_OFONO)
#include "qofonowrapper_p.h"
#endif

#include <QtCore/qdir.h>
#include <QtCore/qfile.h>
#include <QtCore/qmetaobject.h>
#include <QtCore/qtextstream.h>
#include <QtCore/qtimer.h>
#if !defined(QT_NO_BLUEZ)
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#endif // QT_NO_BLUEZ

#if !defined(QT_NO_UDEV)
#include <QtCore/qsocketnotifier.h>
#include <libudev.h>
#endif // QT_NO_UDEV

#include <math.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/wireless.h>
#include <unistd.h>

QT_BEGIN_NAMESPACE

Q_GLOBAL_STATIC_WITH_ARGS(const QString, NETWORK_SYSFS_PATH, (QLatin1String("/sys/class/net/")))

Q_GLOBAL_STATIC_WITH_ARGS(const QStringList, WLAN_MASK, (QStringList() << QLatin1String("wlan*") << QLatin1String("wlp*")))
Q_GLOBAL_STATIC_WITH_ARGS(const QStringList, ETHERNET_MASK, (QStringList() << QLatin1String("eth*") << QLatin1String("usb*") << QLatin1String("enp*")))

QNetworkInfoPrivate::QNetworkInfoPrivate(QNetworkInfo *parent)
    : QObject(parent)
    , q_ptr(parent)
    , watchCurrentNetworkMode(false)
    , watchNetworkInterfaceCount(false)
    , watchNetworkSignalStrength(false)
    , watchNetworkStatus(false)
    , watchNetworkName(false)
    , timer(0)
#if !defined(QT_NO_OFONO)
    , ofonoWrapper(0)
#endif
#if !defined(QT_NO_UDEV)
    , udevNotifier(0)
    , udevHandle(0)
    , udevMonitor(0)
#endif // QT_NO_UDEV
{
}

QNetworkInfoPrivate::~QNetworkInfoPrivate()
{
#if !defined(QT_NO_UDEV)
    if (udevMonitor)
        udev_monitor_unref(udevMonitor);

    if (udevHandle)
        udev_unref(udevHandle);
#endif // QT_NO_UDEV
}

int QNetworkInfoPrivate::networkInterfaceCount(QNetworkInfo::NetworkMode mode)
{
    if (watchNetworkInterfaceCount && (mode == QNetworkInfo::WlanMode
                                       || mode == QNetworkInfo::EthernetMode
                                       || mode == QNetworkInfo::BluetoothMode)) {
        return networkInterfaceCounts.value(mode);
    } else
        return getNetworkInterfaceCount(mode);
}

int QNetworkInfoPrivate::networkSignalStrength(QNetworkInfo::NetworkMode mode, int interface)
{
    if (watchNetworkSignalStrength && (mode == QNetworkInfo::WlanMode
                                       || mode == QNetworkInfo::EthernetMode
                                       || mode == QNetworkInfo::BluetoothMode)) {
        return networkSignalStrengths.value(QPair<QNetworkInfo::NetworkMode, int>(mode, interface));
    } else
        return getNetworkSignalStrength(mode, interface);
}

QNetworkInfo::CellDataTechnology QNetworkInfoPrivate::currentCellDataTechnology(int interface)
{
#if !defined(QT_NO_OFONO)
    if (QOfonoWrapper::isOfonoAvailable()) {
        if (!ofonoWrapper)
            ofonoWrapper = new QOfonoWrapper(this);
        QStringList modems = ofonoWrapper->allModems();
        if (interface < modems.size()) {
            QString modem = ofonoWrapper->allModems().at(interface);
            if (!modem.isEmpty())
                return ofonoWrapper->currentCellDataTechnology(modem);
        }
    }
#else
    Q_UNUSED(interface)
#endif
    return QNetworkInfo::UnknownDataTechnology;
}

QNetworkInfo::NetworkMode QNetworkInfoPrivate::currentNetworkMode()
{
    if (watchCurrentNetworkMode)
        return currentMode;
    else
        return getCurrentNetworkMode();
}

QNetworkInfo::NetworkStatus QNetworkInfoPrivate::networkStatus(QNetworkInfo::NetworkMode mode, int interface)
{
    if (watchNetworkStatus && (mode == QNetworkInfo::WlanMode
                               || mode == QNetworkInfo::EthernetMode
                               || mode == QNetworkInfo::BluetoothMode)) {
        return networkStatuses.value(QPair<QNetworkInfo::NetworkMode, int>(mode, interface));
    } else
        return getNetworkStatus(mode, interface);
}

#ifndef QT_NO_NETWORKINTERFACE
QNetworkInterface QNetworkInfoPrivate::interfaceForMode(QNetworkInfo::NetworkMode mode, int interface)
{
    switch (mode) {
    case QNetworkInfo::WlanMode: {
        QStringList dirs = QDir(*NETWORK_SYSFS_PATH()).entryList(*WLAN_MASK());
        if (interface < dirs.size()) {
            QNetworkInterface networkInterface = QNetworkInterface::interfaceFromName(dirs.at(interface));
            if (networkInterface.isValid())
                return networkInterface;
        }
        break;
    }

    case QNetworkInfo::EthernetMode: {
        QStringList dirs = QDir(*NETWORK_SYSFS_PATH()).entryList(*ETHERNET_MASK());
        if (interface < dirs.size()) {
            QNetworkInterface networkInterface = QNetworkInterface::interfaceFromName(dirs.at(interface));
            if (networkInterface.isValid())
                return networkInterface;
        }
        break;
    }

//    case QNetworkInfo::BluetoothMode:
//    case QNetworkInfo::GsmMode:
//    case QNetworkInfo::CdmaMode:
//    case QNetworkInfo::WcdmaMode:
//    case QNetworkInfo::WimaxMode:
//    case QNetworkInfo::LteMode:
//    case QNetworkInfo::TdscdmaMode:
    default:
        break;
    };

    return QNetworkInterface();
}
#endif // QT_NO_NETWORKINTERFACE

QString QNetworkInfoPrivate::cellId(int interface)
{
#if !defined(QT_NO_OFONO)
    if (QOfonoWrapper::isOfonoAvailable()) {
        if (!ofonoWrapper)
            ofonoWrapper = new QOfonoWrapper(this);
        QStringList modems = ofonoWrapper->allModems();
        if (interface < modems.size()) {
            QString modem = ofonoWrapper->allModems().at(interface);
            if (!modem.isEmpty())
                return ofonoWrapper->cellId(modem);
        }
    }
#else
    Q_UNUSED(interface)
#endif
    return QString();
}

QString QNetworkInfoPrivate::currentMobileCountryCode(int interface)
{
#if !defined(QT_NO_OFONO)
    if (QOfonoWrapper::isOfonoAvailable()) {
        if (!ofonoWrapper)
            ofonoWrapper = new QOfonoWrapper(this);
        QStringList modems = ofonoWrapper->allModems();
        if (interface < modems.size()) {
            QString modem = ofonoWrapper->allModems().at(interface);
            if (!modem.isEmpty())
                return ofonoWrapper->currentMcc(modem);
        }
    }
#else
    Q_UNUSED(interface)
#endif
    return QString();
}

QString QNetworkInfoPrivate::currentMobileNetworkCode(int interface)
{
#if !defined(QT_NO_OFONO)
    if (QOfonoWrapper::isOfonoAvailable()) {
        if (!ofonoWrapper)
            ofonoWrapper = new QOfonoWrapper(this);
        QStringList modems = ofonoWrapper->allModems();
        if (interface < modems.size()) {
            QString modem = ofonoWrapper->allModems().at(interface);
            if (!modem.isEmpty())
                return ofonoWrapper->currentMnc(modem);
        }
    }
#else
    Q_UNUSED(interface)
#endif
    return QString();
}

QString QNetworkInfoPrivate::homeMobileCountryCode(int interface)
{
#if !defined(QT_NO_OFONO)
    if (QOfonoWrapper::isOfonoAvailable()) {
        if (!ofonoWrapper)
            ofonoWrapper = new QOfonoWrapper(this);
        QStringList modems = ofonoWrapper->allModems();
        if (interface < modems.size()) {
            QString modem = ofonoWrapper->allModems().at(interface);
            if (!modem.isEmpty())
                return ofonoWrapper->homeMcc(modem);
        }
    }
#else
    Q_UNUSED(interface)
#endif
    return QString();
}

QString QNetworkInfoPrivate::homeMobileNetworkCode(int interface)
{
#if !defined(QT_NO_OFONO)
    if (QOfonoWrapper::isOfonoAvailable()) {
        if (!ofonoWrapper)
            ofonoWrapper = new QOfonoWrapper(this);
        QStringList modems = ofonoWrapper->allModems();
        if (interface < modems.size()) {
            QString modem = ofonoWrapper->allModems().at(interface);
            if (!modem.isEmpty())
                return ofonoWrapper->homeMnc(modem);
        }
    }
#else
    Q_UNUSED(interface)
#endif
    return QString();
}

QString QNetworkInfoPrivate::imsi(int interface)
{
#if !defined(QT_NO_OFONO)
    if (QOfonoWrapper::isOfonoAvailable()) {
        if (!ofonoWrapper)
            ofonoWrapper = new QOfonoWrapper(this);
        QStringList modems = ofonoWrapper->allModems();
        if (interface < modems.size()) {
            QString modem = ofonoWrapper->allModems().at(interface);
            if (!modem.isEmpty())
                return ofonoWrapper->imsi(modem);
        }
    }
#else
    Q_UNUSED(interface)
#endif
    return QString();
}

QString QNetworkInfoPrivate::locationAreaCode(int interface)
{
#if !defined(QT_NO_OFONO)
    if (QOfonoWrapper::isOfonoAvailable()) {
        if (!ofonoWrapper)
            ofonoWrapper = new QOfonoWrapper(this);
        QStringList modems = ofonoWrapper->allModems();
        if (interface < modems.size()) {
            QString modem = ofonoWrapper->allModems().at(interface);
            if (!modem.isEmpty())
                return ofonoWrapper->lac(modem);
        }
    }
#else
    Q_UNUSED(interface)
#endif
    return QString();
}

QString QNetworkInfoPrivate::macAddress(QNetworkInfo::NetworkMode mode, int interface)
{
    switch (mode) {
    case QNetworkInfo::WlanMode: {
        QStringList dirs = QDir(*NETWORK_SYSFS_PATH()).entryList(*WLAN_MASK());
        if (interface < dirs.size()) {
            QFile carrier(*NETWORK_SYSFS_PATH() + dirs.at(interface) + QString(QStringLiteral("/address")));
            if (carrier.open(QIODevice::ReadOnly))
                return QString::fromLatin1(carrier.readAll().simplified().data());
        }
        break;
    }

    case QNetworkInfo::EthernetMode: {
        QStringList dirs = QDir(*NETWORK_SYSFS_PATH()).entryList(*ETHERNET_MASK());
        if (interface < dirs.size()) {
            QFile carrier(*NETWORK_SYSFS_PATH() + dirs.at(interface) + QString(QStringLiteral("/address")));
            if (carrier.open(QIODevice::ReadOnly))
                return QString::fromLatin1(carrier.readAll().simplified().data());
        }
        break;
    }

    case QNetworkInfo::BluetoothMode: {
#if !defined(QT_NO_BLUEZ)
        int ctl = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI);
        if (ctl < 0)
            break;
        struct hci_dev_list_req *deviceList = (struct hci_dev_list_req *)malloc(HCI_MAX_DEV * sizeof(struct hci_dev_req) + sizeof(uint16_t));
        deviceList->dev_num = HCI_MAX_DEV;
        QString macAddress;
        if (ioctl(ctl, HCIGETDEVLIST, deviceList) == 0) {
            int count = deviceList->dev_num;
            if (interface < count) {
                struct hci_dev_info deviceInfo;
                deviceInfo.dev_id = (deviceList->dev_req + interface)->dev_id;
                if (ioctl(ctl, HCIGETDEVINFO, &deviceInfo) == 0) {
                    // do not use BDADDR_ANY, fails with gcc 4.6
                    bdaddr_t bdaddr_any = (bdaddr_t) {{0, 0, 0, 0, 0, 0}};
                    if (hci_test_bit(HCI_RAW, &deviceInfo.flags) && !bacmp(&deviceInfo.bdaddr, &bdaddr_any)) {
                        int hciDevice = hci_open_dev(deviceInfo.dev_id);
                        hci_read_bd_addr(hciDevice, &deviceInfo.bdaddr, 1000);
                        hci_close_dev(hciDevice);
                    }
                    char address[18];
                    ba2str(&deviceInfo.bdaddr, address);
                    macAddress = QString::fromLatin1(address);
                }
            }
        }
        free(deviceList);
        close(ctl);
        return macAddress;
#else
        break;
#endif // QT_NO_BLUEZ
    }

//    case QNetworkInfo::GsmMode:
//    case QNetworkInfo::CdmaMode:
//    case QNetworkInfo::WcdmaMode:
//    case QNetworkInfo::WimaxMode:
//    case QNetworkInfo::LteMode:
//    case QNetworkInfo::TdscdmaMode:
    default:
        break;
    };

    return QString();
}

QString QNetworkInfoPrivate::networkName(QNetworkInfo::NetworkMode mode, int interface)
{
    if (watchNetworkName && (mode == QNetworkInfo::WlanMode
                             || mode == QNetworkInfo::EthernetMode
                             || mode == QNetworkInfo::BluetoothMode)) {
        return networkNames.value(QPair<QNetworkInfo::NetworkMode, int>(mode, interface));
    } else
        return getNetworkName(mode, interface);
}

extern QMetaMethod proxyToSourceSignal(const QMetaMethod &, QObject *);

void QNetworkInfoPrivate::connectNotify(const QMetaMethod &signal)
{
#if !defined(QT_NO_OFONO)
    if (QOfonoWrapper::isOfonoAvailable()) {
        if (!ofonoWrapper)
            ofonoWrapper = new QOfonoWrapper(this);
        QMetaMethod sourceSignal = proxyToSourceSignal(signal, ofonoWrapper);
        connect(ofonoWrapper, sourceSignal, this, signal, Qt::UniqueConnection);
    }
#endif

    static const QMetaMethod currentNetworkModeChangedSignal = QMetaMethod::fromSignal(&QNetworkInfoPrivate::currentNetworkModeChanged);
    static const QMetaMethod networkNameChangedSignal = QMetaMethod::fromSignal(&QNetworkInfoPrivate::networkNameChanged);
    static const QMetaMethod networkSignalStrengthChangedSignal = QMetaMethod::fromSignal(&QNetworkInfoPrivate::networkSignalStrengthChanged);
    static const QMetaMethod networkStatusChangedSignal = QMetaMethod::fromSignal(&QNetworkInfoPrivate::networkStatusChanged);

    //    we always monitor "networkInterfaceCount" , as long as users connect any signals,
    //    with update to date network interface counts, we can compute network mode, strength,
    //    status, name properties in an efficient way
    if (!watchNetworkInterfaceCount) {
        QList<QNetworkInfo::NetworkMode> modes;
        modes << QNetworkInfo::WlanMode << QNetworkInfo::EthernetMode << QNetworkInfo::BluetoothMode;
        networkInterfaceCounts.clear();
        foreach (QNetworkInfo::NetworkMode mode, modes)
            networkInterfaceCounts[mode] = getNetworkInterfaceCount(mode);
#if !defined(QT_NO_UDEV)
        if (!udevHandle) {
            udevHandle = udev_new();
            udevMonitor = udev_monitor_new_from_netlink(udevHandle, "udev");
            if (udevMonitor) {
                udev_monitor_filter_add_match_subsystem_devtype(udevMonitor, "net", NULL);
                udev_monitor_enable_receiving(udevMonitor);
                udevNotifier = new QSocketNotifier(udev_monitor_get_fd(udevMonitor), QSocketNotifier::Read, this);
                connect(udevNotifier, SIGNAL(activated(int)), this, SLOT(onUdevChanged()));
            }
        }
        if (udevNotifier) {
            udevNotifier->setEnabled(true);
        }

#endif // QT_NO_UDEV
        watchNetworkInterfaceCount = true;
    }

    if (signal == currentNetworkModeChangedSignal) {
//        we monitor "networkStatus" by default, as long as currentNetworkModeChanged signal
//        is connected, with always up to date network status, current network mode can
//        be fast computed.
        if (!watchNetworkStatus) {
            QList<QNetworkInfo::NetworkMode> modes;
            modes << QNetworkInfo::WlanMode << QNetworkInfo::EthernetMode << QNetworkInfo::BluetoothMode;
            networkStatuses.clear();
            foreach (QNetworkInfo::NetworkMode mode, modes) {
                int count = networkInterfaceCount(mode);
                for (int i = 0; i < count; ++i)
                    networkStatuses[QPair<QNetworkInfo::NetworkMode, int>(mode, i)] = getNetworkStatus(mode, i);
            }
            watchNetworkStatus = true;
        }
        currentMode = getCurrentNetworkMode();
        watchCurrentNetworkMode = true;
    } else if (signal == networkSignalStrengthChangedSignal) {
        QList<QNetworkInfo::NetworkMode> modes;
        modes << QNetworkInfo::WlanMode << QNetworkInfo::EthernetMode << QNetworkInfo::BluetoothMode;
        networkSignalStrengths.clear();
        foreach (QNetworkInfo::NetworkMode mode, modes) {
            int count = networkInterfaceCount(mode);
            for (int i = 0; i < count; ++i)
                networkSignalStrengths[QPair<QNetworkInfo::NetworkMode, int>(mode, i)] = getNetworkSignalStrength(mode, i);
        }

        watchNetworkSignalStrength = true;
    } else if (signal == networkStatusChangedSignal) {
        QList<QNetworkInfo::NetworkMode> modes;
        modes << QNetworkInfo::WlanMode << QNetworkInfo::EthernetMode << QNetworkInfo::BluetoothMode;
        networkStatuses.clear();
        foreach (QNetworkInfo::NetworkMode mode, modes) {
            int count = networkInterfaceCount(mode);
            for (int i = 0; i < count; ++i)
                networkStatuses[QPair<QNetworkInfo::NetworkMode, int>(mode, i)] = getNetworkStatus(mode, i);
        }

        watchNetworkStatus = true;
    } else if (signal == networkNameChangedSignal) {
        QList<QNetworkInfo::NetworkMode> modes;
        modes << QNetworkInfo::WlanMode << QNetworkInfo::EthernetMode << QNetworkInfo::BluetoothMode;
        networkNames.clear();
        foreach (QNetworkInfo::NetworkMode mode, modes) {
            int count = networkInterfaceCount(mode);
            for (int i = 0; i < count; ++i)
                networkNames[QPair<QNetworkInfo::NetworkMode, int>(mode, i)] = getNetworkName(mode, i);
        }

        watchNetworkName = true;
    } else if (signal == currentNetworkModeChangedSignal) {
        currentMode = getCurrentNetworkMode();
        watchCurrentNetworkMode = true;
    } else {
        return;
    }

    if (!timer) {
        timer = new QTimer(this);
        timer->setInterval(2000);
        connect(timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    }

    if (!timer->isActive())
        timer->start();
}

void QNetworkInfoPrivate::disconnectNotify(const QMetaMethod &signal)
{
#if !defined(QT_NO_OFONO)
    if (!QOfonoWrapper::isOfonoAvailable() || !ofonoWrapper)
        return;

    {
        QMetaMethod sourceSignal = proxyToSourceSignal(signal, ofonoWrapper);
        disconnect(ofonoWrapper, sourceSignal, this, signal);
    }
#endif

    static const QMetaMethod currentNetworkModeChangedSignal = QMetaMethod::fromSignal(&QNetworkInfoPrivate::currentNetworkModeChanged);
    static const QMetaMethod networkInterfaceCountChangedSignal = QMetaMethod::fromSignal(&QNetworkInfoPrivate::networkInterfaceCountChanged);
    static const QMetaMethod networkNameChangedSignal = QMetaMethod::fromSignal(&QNetworkInfoPrivate::networkNameChanged);
    static const QMetaMethod networkSignalStrengthChangedSignal = QMetaMethod::fromSignal(&QNetworkInfoPrivate::networkSignalStrengthChanged);
    static const QMetaMethod networkStatusChangedSignal = QMetaMethod::fromSignal(&QNetworkInfoPrivate::networkStatusChanged);

    if (signal == networkInterfaceCountChangedSignal
            && !watchNetworkStatus && !watchNetworkName && !watchNetworkSignalStrength ) {
#if !defined(QT_NO_UDEV)
        if (udevNotifier) {
            udevNotifier->setEnabled(false);
        }
        watchNetworkInterfaceCount = false;
        return;
#endif // QT_NO_UDEV
        watchNetworkInterfaceCount = false;
    } else if (signal == networkSignalStrengthChangedSignal) {
        watchNetworkSignalStrength = false;
    } else if ((!watchCurrentNetworkMode) && (signal == networkStatusChangedSignal)) {
        watchNetworkStatus = false;
    } else if (signal == networkNameChangedSignal) {
        watchNetworkName = false;
    } else if (signal == currentNetworkModeChangedSignal) {
        watchCurrentNetworkMode = false;
    } else {
        return;
    }

    if (!watchNetworkInterfaceCount && !watchNetworkSignalStrength && !watchNetworkStatus && !watchNetworkName && !watchCurrentNetworkMode)
        timer->stop();
}

#if !defined(QT_NO_UDEV)
void QNetworkInfoPrivate::onUdevChanged()
{
    struct udev_device *udevDevice = udev_monitor_receive_device(udevMonitor);
    if (!udevDevice)
        return;

    if (0 != strcmp(udev_device_get_subsystem(udevDevice), "net"))
        return;

    QString sysname(QString::fromLocal8Bit(udev_device_get_sysname(udevDevice)));
    if (watchNetworkInterfaceCount) {
        if (sysname.startsWith(QLatin1String("eth"))
                || sysname.startsWith(QLatin1String("usb"))
                || sysname.startsWith(QLatin1String("enp"))) {
            if (0 == strcmp(udev_device_get_action(udevDevice), "add"))
                ++networkInterfaceCounts[QNetworkInfo::EthernetMode];
            else if (0 == strcmp(udev_device_get_action(udevDevice), "remove"))
                --networkInterfaceCounts[QNetworkInfo::EthernetMode];
            emit networkInterfaceCountChanged(QNetworkInfo::EthernetMode,
                                                networkInterfaceCounts[QNetworkInfo::EthernetMode]);
        } else if (sysname.startsWith(QLatin1String("wlan")) || sysname.startsWith(QLatin1String("wlp"))) {
            if (0 == strcmp(udev_device_get_action(udevDevice), "add"))
                ++networkInterfaceCounts[QNetworkInfo::WlanMode];
            else if (0 == strcmp(udev_device_get_action(udevDevice), "remove"))
                --networkInterfaceCounts[QNetworkInfo::WlanMode];
            emit networkInterfaceCountChanged(QNetworkInfo::WlanMode,
                                                networkInterfaceCounts[QNetworkInfo::WlanMode]);
        }
    }

    udev_device_unref(udevDevice);
}
#endif // QT_NO_UDEV

void QNetworkInfoPrivate::onTimeout()
{
#if defined(QT_NO_UDEV)
    if (watchNetworkInterfaceCount) {
        QList<QNetworkInfo::NetworkMode> modes;
        modes << QNetworkInfo::WlanMode << QNetworkInfo::EthernetMode << QNetworkInfo::BluetoothMode;
        foreach (QNetworkInfo::NetworkMode mode, modes) {
            int value = getNetworkInterfaceCount(mode);
            if (networkInterfaceCounts.value(mode) != value) {
                networkInterfaceCounts[mode] = value;
                emit networkInterfaceCountChanged(mode, value);
            }
        }
    }
#endif // QT_NO_UDEV

    if (!watchNetworkSignalStrength && !watchNetworkStatus && !watchNetworkName && !watchCurrentNetworkMode)
        return;

    QList<QNetworkInfo::NetworkMode> modes;
    modes << QNetworkInfo::WlanMode << QNetworkInfo::EthernetMode << QNetworkInfo::BluetoothMode;
    foreach (QNetworkInfo::NetworkMode mode, modes) {
        int count = networkInterfaceCount(mode);
        for (int i = 0; i < count; ++i) {
            if (watchNetworkSignalStrength) {
                int value = getNetworkSignalStrength(mode, i);
                QPair<QNetworkInfo::NetworkMode, int> key(mode, i);
                if (networkSignalStrengths.value(key) != value) {
                    networkSignalStrengths[key] = value;
                    emit networkSignalStrengthChanged(mode, i, value);
                }
            }

            if (watchNetworkStatus) {
                QNetworkInfo::NetworkStatus value = getNetworkStatus(mode, i);
                QPair<QNetworkInfo::NetworkMode, int> key(mode, i);
                if (networkStatuses.value(key) != value) {
                    networkStatuses[key] = value;
                    emit networkStatusChanged(mode, i, value);
                }
            }

            if (watchNetworkName) {
                QString value = getNetworkName(mode, i);
                QPair<QNetworkInfo::NetworkMode, int> key(mode, i);
                if (networkNames.value(key) != value) {
                    networkNames[key] = value;
                    emit networkNameChanged(mode, i, value);
                }
            }
        }
    }

    if (watchCurrentNetworkMode) {
        QNetworkInfo::NetworkMode value = getCurrentNetworkMode();
        if (currentMode != value) {
            currentMode = value;
            emit currentNetworkModeChanged(value);
        }
    }

}

int QNetworkInfoPrivate::getNetworkInterfaceCount(QNetworkInfo::NetworkMode mode)
{
    switch (mode) {
    case QNetworkInfo::WlanMode:
        return QDir(*NETWORK_SYSFS_PATH()).entryList(*WLAN_MASK()).size();

    case QNetworkInfo::EthernetMode:
        return QDir(*NETWORK_SYSFS_PATH()).entryList(*ETHERNET_MASK()).size();

    case QNetworkInfo::BluetoothMode: {
        int count = -1;
#if !defined(QT_NO_BLUEZ)
        int ctl = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI);
        if (ctl < 0)
            return count;
        struct hci_dev_list_req *deviceList = (struct hci_dev_list_req *)malloc(HCI_MAX_DEV * sizeof(struct hci_dev_req) + sizeof(uint16_t));
        deviceList->dev_num = HCI_MAX_DEV;
        if (ioctl(ctl, HCIGETDEVLIST, deviceList) == 0)
            count = deviceList->dev_num;
        free(deviceList);
        close(ctl);
#endif // QT_NO_BLUEZ
        return count;
    }

    case QNetworkInfo::GsmMode:
    case QNetworkInfo::CdmaMode:
    case QNetworkInfo::WcdmaMode:
    case QNetworkInfo::LteMode:
    case QNetworkInfo::TdscdmaMode:
#if !defined(QT_NO_OFONO)
        if (QOfonoWrapper::isOfonoAvailable()) {
            if (!ofonoWrapper)
                ofonoWrapper = new QOfonoWrapper(this);
            return ofonoWrapper->allModems().size();
        }
#endif

//    case QNetworkInfo::WimaxMode:
    default:
        return -1;
    };
}

int QNetworkInfoPrivate::getNetworkSignalStrength(QNetworkInfo::NetworkMode mode, int interface)
{
    switch (mode) {
    case QNetworkInfo::WlanMode: {
        QFile file(QString(QStringLiteral("/proc/net/wireless")));
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return -1;

        QTextStream in(&file);
        QString interfaceName = interfaceForMode(QNetworkInfo::WlanMode, interface).name();

        QStringList lines = in.readAll().split(QStringLiteral("\n"));
        for (int i = 0; i < lines.size(); i++) {
            QString line = lines.at(i);
            if (!line.isNull() && line.left(6).contains(interfaceName)) {
                // A typical wireless received signal power over a network falls into the range of (-120, -20),
                // we shift the dbm value, which is read from the field "Quality - level" in "/proc/net/wireless",
                // from (-120, -20) to (0, 100) by adding 120. In case of outliers, we restrict them to the
                // corresponding boundary value.
                QString token = line.section(QString(QStringLiteral(" ")), 3, 3, QString::SectionSkipEmpty).simplified();
                token.chop(1);
                bool ok;
                int signalStrength = token.toInt(&ok);
                if (ok) {
                    signalStrength += 120;
                    if (signalStrength > 100)
                        signalStrength = 100;
                    else if (signalStrength < 0)
                        signalStrength = 0;
                    return signalStrength;
                } else {
                    return -1;
                }
            }
        }

        break;
    }

    case QNetworkInfo::EthernetMode:
        if (networkStatus(QNetworkInfo::EthernetMode, interface) == QNetworkInfo::HomeNetwork)
            return 100;
        else
            return -1;

    case QNetworkInfo::GsmMode:
    case QNetworkInfo::CdmaMode:
    case QNetworkInfo::WcdmaMode:
    case QNetworkInfo::LteMode:
    case QNetworkInfo::TdscdmaMode:
#if !defined(QT_NO_OFONO)
        if (QOfonoWrapper::isOfonoAvailable()) {
            if (!ofonoWrapper)
                ofonoWrapper = new QOfonoWrapper(this);
            QStringList modems = ofonoWrapper->allModems();
            if (interface < modems.size()) {
                QString modem = ofonoWrapper->allModems().at(interface);
                if (!modem.isEmpty())
                    return ofonoWrapper->signalStrength(modem);
            }
        }
#endif
        break;

    case QNetworkInfo::BluetoothMode: {
        int signalStrength = -1;
#if !defined(QT_NO_BLUEZ)
        int ctl = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI);
        if (ctl < 0)
            break;
        struct hci_dev_list_req *deviceList = (struct hci_dev_list_req *)malloc(HCI_MAX_DEV * sizeof(struct hci_dev_req) + sizeof(uint16_t));
        deviceList->dev_num = HCI_MAX_DEV;
        if (ioctl(ctl, HCIGETDEVLIST, deviceList) == 0) {
            int count = deviceList->dev_num;
            if (interface < count) {
                signalStrength = 0; // Valid interface

                struct hci_conn_list_req *connectionList = (struct hci_conn_list_req *)malloc(sizeof(struct hci_conn_info) + sizeof(struct hci_conn_list_req));
                connectionList->dev_id = (deviceList->dev_req + interface)->dev_id;
                connectionList->conn_num = 1;
                if (ioctl(ctl, HCIGETCONNLIST, connectionList) == 0) {
                    if (connectionList->conn_num == 1) {
                        int fd = hci_open_dev((deviceList->dev_req + interface)->dev_id);
                        if (fd > 0) {
                            struct hci_conn_info_req *connectionInfo = (struct hci_conn_info_req *)malloc(sizeof(struct hci_conn_info_req) + sizeof(struct hci_conn_info));
                            bacpy(&connectionInfo->bdaddr, &connectionList->conn_info->bdaddr);
                            connectionInfo->type = ACL_LINK;
                            if (ioctl(fd, HCIGETCONNINFO, connectionInfo) == 0) {
                                uint8_t linkQuality;
                                if (hci_read_link_quality(fd, htobs(connectionInfo->conn_info->handle), &linkQuality, 0) == 0)
                                    signalStrength = linkQuality * 100 / 255;
                            }
                            free(connectionInfo);
                        }
                    }
                }
                free (connectionList);
            }
        }
        free(deviceList);
        close(ctl);
#endif // QT_NO_BLUEZ
        return signalStrength;
    }

//    case QNetworkInfo::WimaxMode:
    default:
        break;
    };

    return -1;
}

QNetworkInfo::NetworkMode QNetworkInfoPrivate::getCurrentNetworkMode()
{
    // TODO multiple-interface support
    if (networkStatus(QNetworkInfo::EthernetMode, 0) == QNetworkInfo::HomeNetwork)
        return QNetworkInfo::EthernetMode;
    else if (networkStatus(QNetworkInfo::WlanMode, 0) == QNetworkInfo::HomeNetwork)
        return QNetworkInfo::WlanMode;
    else if (networkStatus(QNetworkInfo::BluetoothMode, 0) == QNetworkInfo::HomeNetwork)
        return QNetworkInfo::BluetoothMode;
    else if (networkStatus(QNetworkInfo::WimaxMode, 0) == QNetworkInfo::HomeNetwork)
        return QNetworkInfo::WimaxMode;
    else if (networkStatus(QNetworkInfo::LteMode, 0) == QNetworkInfo::HomeNetwork)
        return QNetworkInfo::LteMode;
    else if (networkStatus(QNetworkInfo::WcdmaMode, 0) == QNetworkInfo::HomeNetwork)
        return QNetworkInfo::WcdmaMode;
    else if (networkStatus(QNetworkInfo::CdmaMode, 0) == QNetworkInfo::HomeNetwork)
        return QNetworkInfo::CdmaMode;
    else if (networkStatus(QNetworkInfo::GsmMode, 0) == QNetworkInfo::HomeNetwork)
        return QNetworkInfo::GsmMode;
    else if (networkStatus(QNetworkInfo::TdscdmaMode, 0) == QNetworkInfo::HomeNetwork)
        return QNetworkInfo::TdscdmaMode;
    else if (networkStatus(QNetworkInfo::WimaxMode, 0) == QNetworkInfo::Roaming)
        return QNetworkInfo::WimaxMode;
    else if (networkStatus(QNetworkInfo::LteMode, 0) == QNetworkInfo::Roaming)
        return QNetworkInfo::LteMode;
    else if (networkStatus(QNetworkInfo::WcdmaMode, 0) == QNetworkInfo::Roaming)
        return QNetworkInfo::WcdmaMode;
    else if (networkStatus(QNetworkInfo::CdmaMode, 0) == QNetworkInfo::Roaming)
        return QNetworkInfo::CdmaMode;
    else if (networkStatus(QNetworkInfo::GsmMode, 0) == QNetworkInfo::Roaming)
        return QNetworkInfo::GsmMode;
    else if (networkStatus(QNetworkInfo::TdscdmaMode, 0) == QNetworkInfo::Roaming)
        return QNetworkInfo::TdscdmaMode;
    else
        return QNetworkInfo::UnknownMode;
}

QNetworkInfo::NetworkStatus QNetworkInfoPrivate::getNetworkStatus(QNetworkInfo::NetworkMode mode, int interface)
{
    switch (mode) {
    case QNetworkInfo::WlanMode: {
        if (interface < networkInterfaceCount(QNetworkInfo::WlanMode)) {
            const QString fileName = QDir(*NETWORK_SYSFS_PATH()).entryList(*WLAN_MASK()).at(interface);
            QFile carrier(*NETWORK_SYSFS_PATH() + fileName + QStringLiteral("/carrier"));
            if (carrier.open(QIODevice::ReadOnly)) {
                char state;
                if ((carrier.read(&state, 1) == 1) &&
                        (state == '1')) {
                    return QNetworkInfo::HomeNetwork;
                }
            }
        }
        return QNetworkInfo::NoNetworkAvailable;
    }

    case QNetworkInfo::EthernetMode: {
        if (interface < networkInterfaceCount(QNetworkInfo::EthernetMode)) {
            const QString fileName = QDir(*NETWORK_SYSFS_PATH()).entryList(*ETHERNET_MASK()).at(interface);
            QFile carrier(*NETWORK_SYSFS_PATH() + fileName + QStringLiteral("/carrier"));
            if (carrier.open(QIODevice::ReadOnly)) {
                char state;
                if ((carrier.read(&state, 1) == 1) && (state == '1'))
                    return QNetworkInfo::HomeNetwork;
            }
        }
        return QNetworkInfo::NoNetworkAvailable;
    }

    case QNetworkInfo::BluetoothMode: {
        QNetworkInfo::NetworkStatus status = QNetworkInfo::UnknownStatus;

#if !defined(QT_NO_BLUEZ)
        int ctl = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI);
        if (ctl < 0)
            break;
        struct hci_dev_list_req *deviceList = (struct hci_dev_list_req *)malloc(HCI_MAX_DEV * sizeof(struct hci_dev_req) + sizeof(uint16_t));
        deviceList->dev_num = HCI_MAX_DEV;
        if (ioctl(ctl, HCIGETDEVLIST, deviceList) == 0) {
            int count = deviceList->dev_num;
            if (interface < count) {
                status = QNetworkInfo::NoNetworkAvailable; // Valid interface, so either not connected or connected
                                                           // TODO add support for searching and denied
                struct hci_conn_list_req *connectionList = (struct hci_conn_list_req *)malloc(sizeof(struct hci_conn_info) + sizeof(struct hci_conn_list_req));
                connectionList->dev_id = (deviceList->dev_req + interface)->dev_id;
                connectionList->conn_num = 1;
                if (ioctl(ctl, HCIGETCONNLIST, connectionList) == 0) {
                    if (connectionList->conn_num == 1)
                        status = QNetworkInfo::HomeNetwork;
                }
                free (connectionList);
            }
        }
        free(deviceList);
        close(ctl);
#endif // QT_NO_BLUEZ

        return status;
    }

    case QNetworkInfo::GsmMode:
    case QNetworkInfo::CdmaMode:
    case QNetworkInfo::WcdmaMode:
    case QNetworkInfo::LteMode:
    case QNetworkInfo::TdscdmaMode:
#if !defined(QT_NO_OFONO)
        if (QOfonoWrapper::isOfonoAvailable()) {
            if (!ofonoWrapper)
                ofonoWrapper = new QOfonoWrapper(this);
            QStringList modems = ofonoWrapper->allModems();
            if (interface < modems.size()) {
                QString modem = ofonoWrapper->allModems().at(interface);
                if (!modem.isEmpty())
                    return ofonoWrapper->networkStatus(modem);
            }
    }
#endif
        break;

//    case QNetworkInfo::WimaxMode:
    default:
        break;
    };

    return QNetworkInfo::UnknownStatus;
}

QString QNetworkInfoPrivate::getNetworkName(QNetworkInfo::NetworkMode mode, int interface)
{
    switch (mode) {
    case QNetworkInfo::WlanMode: {
        if (interface < networkInterfaceCount(QNetworkInfo::WlanMode)) {
            int sock = socket(PF_INET, SOCK_DGRAM, 0);
            if (sock > 0) {
                char buffer[IW_ESSID_MAX_SIZE + 1];
                iwreq iwInfo;

                iwInfo.u.essid.pointer = (caddr_t)&buffer;
                iwInfo.u.essid.length = IW_ESSID_MAX_SIZE + 1;
                iwInfo.u.essid.flags = 0;
                for (int i = 0; i < WLAN_MASK()->count(); i++) {
                    const QString fileName = QDir(*NETWORK_SYSFS_PATH()).entryList(*WLAN_MASK()).at(interface);
                    strncpy(iwInfo.ifr_name, fileName.toLocal8Bit().constData(), IFNAMSIZ);
                    if (ioctl(sock, SIOCGIWESSID, &iwInfo) == 0) {
                        close(sock);
                        return QString::fromLatin1((const char *)iwInfo.u.essid.pointer);
                    } else {
                        qDebug() << "ioctl failed";
                    }

                    close(sock);

                }
            }
        }
        break;
    }

    case QNetworkInfo::EthernetMode: {
        // TODO multiple-interface support
        char domainName[64];
        if (getdomainname(domainName, 64) == 0) {
            if (strcmp(domainName, "(none)") != 0)
                return QString::fromLatin1(domainName);
            else
                return QStringLiteral("Unknown");
        }
        break;
    }

    case QNetworkInfo::BluetoothMode: {
#if !defined(QT_NO_BLUEZ)
        int ctl = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI);
        if (ctl < 0)
            break;
        struct hci_dev_list_req *deviceList = (struct hci_dev_list_req *)malloc(HCI_MAX_DEV * sizeof(struct hci_dev_req) + sizeof(uint16_t));
        deviceList->dev_num = HCI_MAX_DEV;
        QString networkName;
        if (ioctl(ctl, HCIGETDEVLIST, deviceList) == 0) {
            int count = deviceList->dev_num;
            if (interface < count) {
                int fd = hci_open_dev((deviceList->dev_req + interface)->dev_id);
                if (fd > 0) {
                    char name[249];
                    if (hci_read_local_name(fd, sizeof(name), name, 0) == 0)
                        networkName = QString::fromLatin1(name);
                }
            }
        }
        free(deviceList);
        close(ctl);
        return networkName;
#endif // QT_NO_BLUEZ
        break;
    }

    case QNetworkInfo::GsmMode:
    case QNetworkInfo::CdmaMode:
    case QNetworkInfo::WcdmaMode:
    case QNetworkInfo::LteMode:
    case QNetworkInfo::TdscdmaMode:
#if !defined(QT_NO_OFONO)
        if (QOfonoWrapper::isOfonoAvailable()) {
            if (!ofonoWrapper)
                ofonoWrapper = new QOfonoWrapper(this);
            QStringList modems = ofonoWrapper->allModems();
            if (interface < modems.size()) {
                QString modem = ofonoWrapper->allModems().at(interface);
                if (!modem.isEmpty())
                    return ofonoWrapper->operatorName(modem);
            }
        }
#endif
        break;

//    case QNetworkInfo::WimaxMode:
    default:
        break;
    };

    return QString();
}

QT_END_NAMESPACE
