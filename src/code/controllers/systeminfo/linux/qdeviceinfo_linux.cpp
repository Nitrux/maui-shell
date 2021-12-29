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

#include "qdeviceinfo_linux_p.h"

#if !defined(QT_NO_OFONO)
#include "qofonowrapper_p.h"
#endif // QT_NO_OFONO

#if defined(QT_USE_SSU)
#include <ssudeviceinfo.h>
#endif // QT_USE_SSU

#include "qscreensaver_linux_p.h"
#include <QNetworkInfo>

#include <QtCore/qdir.h>
#include <QtCore/qmetaobject.h>
#include <QtCore/qprocess.h>
#include <QtCore/qregularexpression.h>
#include <QtCore/qtextstream.h>
#include <QtCore/qtimer.h>
#include <QtCore/qstandardpaths.h>

#include <fcntl.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <QUuid>
#include <QCryptographicHash>

#if !defined(QT_NO_DBUS)
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusAbstractInterface>
#include <QtDBus/QDBusError>
#include <QtDBus/QDBusMessage>
#endif

QT_BEGIN_NAMESPACE

QDeviceInfoPrivate::QDeviceInfoPrivate(QDeviceInfo *parent)
    : QObject(parent)
#if !defined(QT_SIMULATOR)
    , q_ptr(parent)
#endif // QT_SIMULATOR
    , watchThermalState(false)
    , imeiBuffer(QStringList())
    , uniqueDeviceIDBuffer(QString())
    , timer(0)
    , boardNameString(QString())
    , osName(QString())
#if !defined(QT_NO_OFONO)
        , ofonoWrapper(0)
#endif // QT_NO_OFONO
    ,connectedBtPower(0)
    ,btPowered(0)
{
}

bool QDeviceInfoPrivate::hasFeature(QDeviceInfo::Feature feature)
{
    switch (feature) {
    case QDeviceInfo::BluetoothFeature:
        if (QDir(QStringLiteral("/sys/class/bluetooth/")).entryList(QDir::Dirs | QDir::NoDotAndDotDot).size() > 0)
            return true;
        return false;

    case QDeviceInfo::CameraFeature: {
        const QString devfsPath(QStringLiteral("/dev/"));
        const QStringList dirs = QDir(devfsPath).entryList(QStringList() << QStringLiteral("video*"), QDir::System);
        foreach (const QString &dir, dirs) {
            QFile dev(devfsPath + dir);
            if (!dev.open(QIODevice::ReadWrite))
                continue;
            struct v4l2_capability capability;
            memset(&capability, 0, sizeof(struct v4l2_capability));
            if (ioctl(dev.handle(), VIDIOC_QUERYCAP, &capability) != -1
                && (capability.capabilities & V4L2_CAP_VIDEO_CAPTURE) == V4L2_CAP_VIDEO_CAPTURE) {
                return true;
            }
        }
        return false;
    }

    case QDeviceInfo::FmRadioFeature:
        if (QDir(QStringLiteral("/sys/class/video4linux/")).entryList(QStringList() << QStringLiteral("radio*")).size() > 0)
            return true;
        return false;

    case QDeviceInfo::FmTransmitterFeature: {
        const QString devfsPath(QStringLiteral("/dev/"));
        const QStringList dirs = QDir(devfsPath).entryList(QStringList() << QStringLiteral("radio*"), QDir::System);
        foreach (const QString &dir, dirs) {
            QFile dev(devfsPath + dir);
            if (!dev.open(QIODevice::ReadWrite))
                continue;
            struct v4l2_capability capability;
            memset(&capability, 0, sizeof(struct v4l2_capability));
            if (ioctl(dev.handle(), VIDIOC_QUERYCAP, &capability) != -1
                && (capability.capabilities & (V4L2_CAP_RADIO | V4L2_CAP_MODULATOR)) == (V4L2_CAP_RADIO | V4L2_CAP_MODULATOR)) {
                return true;
            }
        }
        return false;
    }

    case QDeviceInfo::InfraredFeature:
        // TODO: find the kernel interface for this
        return false;

    case QDeviceInfo::LedFeature:
        if (QDir(QStringLiteral("/sys/class/leds/")).entryList(QDir::Dirs | QDir::NoDotAndDotDot).size() > 0)
            return true;
        return false;

    case QDeviceInfo::MemoryCardFeature:
        if (QDir(QStringLiteral("/sys/class/mmc_host/")).entryList(QStringList() << QStringLiteral("mmc*")).size() > 0)
            return true;
        return false;

    case QDeviceInfo::UsbFeature:
        if (QDir(QStringLiteral("/sys/bus/usb/devices/")).entryList(QStringList() << QStringLiteral("usb*")).size() > 0)
            return true;
        return false;

    case QDeviceInfo::VibrationFeature:
        if (QDir(QStringLiteral("/sys/bus/platform/devices/")).entryList(QStringList() << QStringLiteral("*vib*")).size() > 0)
            return true;
        return false;

    case QDeviceInfo::WlanFeature:
        if ((QDir(QStringLiteral("/sys/class/ieee80211/")).entryList(QDir::Dirs | QDir::NoDotAndDotDot).size() > 0)
                || (QDir(QStringLiteral("/sys/class/net/")).entryList(QStringList() << QStringLiteral("wlan*")).size() > 0))
            return true;
        return false;

    case QDeviceInfo::SimFeature:
#if !defined(QT_NO_OFONO)
    if (QOfonoWrapper::isOfonoAvailable()) {
        if (!ofonoWrapper)
            ofonoWrapper = new QOfonoWrapper(this);
        return (ofonoWrapper->allModems().size() > 0);
    }
#endif
        return false;

    case QDeviceInfo::PositioningFeature:
        // TODO: find the kernel interface for this
        if (QDir(QStringLiteral("/sys/class/misc/")).entryList(QStringList() << QStringLiteral("*nmea*")).size() > 0)
            return true;
        return false;

    case QDeviceInfo::VideoOutFeature: {
        const QString devfsPath(QStringLiteral("/dev/"));
        const QStringList dirs = QDir(devfsPath).entryList(QStringList() << QStringLiteral("video*"), QDir::System);
        foreach (const QString &dir, dirs) {
            QFile dev(devfsPath + dir);
            if (!dev.open(QIODevice::ReadWrite))
                continue;
            struct v4l2_capability capability;
            memset(&capability, 0, sizeof(struct v4l2_capability));
            if (ioctl(dev.handle(), VIDIOC_QUERYCAP, &capability) != -1
                && (capability.capabilities & V4L2_CAP_VIDEO_OUTPUT) == V4L2_CAP_VIDEO_OUTPUT) {
                return true;
            }
        }
        if (QDir(QStringLiteral("/sys/class/video_output/")).entryList(QDir::Dirs | QDir::NoDotAndDotDot).size() > 0)
            return true;
        return false;
    }

    case QDeviceInfo::HapticsFeature:
        if (QDir(QStringLiteral("/sys/class/haptic/")).entryList(QDir::Dirs | QDir::NoDotAndDotDot).size() > 0)
            return true;
        return false;

    case QDeviceInfo::NfcFeature:
        // As of now, it's the only supported NFC device in the kernel
        return QFile::exists(QStringLiteral("/dev/pn544"));
    }

    return false;
}

QDeviceInfo::LockTypeFlags QDeviceInfoPrivate::activatedLocks()
{
    return QDeviceInfo::NoLock;
}

QDeviceInfo::LockTypeFlags QDeviceInfoPrivate::enabledLocks()
{
    QDeviceInfo::LockTypeFlags enabledLocks = QDeviceInfo::NoLock;

    QScreenSaverPrivate screenSaver(0);
    if (screenSaver.screenSaverEnabled())
        enabledLocks = QDeviceInfo::TouchOrKeyboardLock;

    return enabledLocks;
}

QDeviceInfo::ThermalState QDeviceInfoPrivate::thermalState()
{
    if (watchThermalState)
        return currentThermalState;
    else
        return getThermalState();
}

int QDeviceInfoPrivate::imeiCount()
{
    if (imeiBuffer.size() == 0)
       imei(0);

    return imeiBuffer.size();
}

QString QDeviceInfoPrivate::imei(int interfaceNumber)
{
#if !defined(QT_NO_OFONO)
    imeiBuffer.clear();
    if (QOfonoWrapper::isOfonoAvailable()) {
        if (!ofonoWrapper)
            ofonoWrapper = new QOfonoWrapper(this);
        QStringList modems = ofonoWrapper->allModems();
        foreach (const QString &modem, modems) {
           if (!modem.isEmpty())
               imeiBuffer += ofonoWrapper->imei(modem);
        }
    }
#else
    Q_UNUSED(interfaceNumber)
#endif
    if (interfaceNumber >= 0 && interfaceNumber < imeiBuffer.size())
       return imeiBuffer[interfaceNumber];
    else
       return QString();
}

QString QDeviceInfoPrivate::manufacturer()
{
#if defined(QT_USE_SSU)
    if (manufacturerBuffer.isEmpty()) {
        manufacturerBuffer = SsuDeviceInfo().displayName(Ssu::DeviceManufacturer);
    }
#endif
    if (manufacturerBuffer.isEmpty()) {
        // for dmi enabled kernels
        QFile file(QStringLiteral("/sys/devices/virtual/dmi/id/sys_vendor"));
        if (file.open(QIODevice::ReadOnly))
            manufacturerBuffer = QString::fromLocal8Bit(file.readAll().simplified().data());
    }
    if (manufacturerBuffer.isEmpty()) {
        QStringList releaseFies = QDir(QStringLiteral("/etc/")).entryList(QStringList() << QStringLiteral("*-release"));
        foreach (const QString &file, releaseFies) {
            if (!manufacturerBuffer.isEmpty())
                continue;
            QFile release(QStringLiteral("/etc/") + file);
            if (release.open(QIODevice::ReadOnly))  {
                QTextStream stream(&release);
                QString line;
                do {
                    line = stream.readLine();
                    //                 this seems to be mer specific
                    if (line.startsWith(QStringLiteral("BUILD"))) {
                      manufacturerBuffer = line.split(QStringLiteral(":")).at(1).simplified().split(QStringLiteral("-")).at(0);
                      break;
                    }
                } while (!line.isNull());
                release.close();
            }
        }
    }
    if (manufacturerBuffer.isEmpty()) {
        manufacturerBuffer = findInRelease(QStringLiteral("BUILD"));
    }
    return manufacturerBuffer;
}

QString QDeviceInfoPrivate::model()
{
#if defined(QT_USE_SSU)
    if (modelBuffer.isEmpty()) {
        modelBuffer = SsuDeviceInfo().displayName(Ssu::DeviceModel);
    }
#endif
    if (modelBuffer.isEmpty()) {
        modelBuffer = findInRelease(QStringLiteral("NAME"),QStringLiteral("hw-release"));
    }
    // for dmi enabled kernels
    if (modelBuffer.isEmpty()) {
        QFile file(QStringLiteral("/sys/devices/virtual/dmi/id/product_name"));
        if (file.open(QIODevice::ReadOnly))
            modelBuffer = QString::fromLocal8Bit(file.readAll().simplified().data());
    }
    if (modelBuffer.isEmpty()) {
        QStringList releaseFies = QDir(QStringLiteral("/etc/")).entryList(QStringList() << QStringLiteral("*-release"));
        foreach (const QString &file, releaseFies) {
            if (!modelBuffer.isEmpty())
                continue;
            QFile release(QStringLiteral("/etc/") + file);
            if (release.open(QIODevice::ReadOnly))  {
                QTextStream stream(&release);
                QString line;
                do {
                    line = stream.readLine();
                    //                 this seems to be mer specific
                    if (line.startsWith(QStringLiteral("BUILD"))) {
                        modelBuffer = line.split(QStringLiteral(":")).at(1).split(QStringLiteral("-")).at(3);
                        break;
                    }
                } while (!line.isNull());
                release.close();
            }
        }
    }
    return modelBuffer;
}

QString QDeviceInfoPrivate::productName()
{
#if defined(QT_USE_SSU)
    if (productNameBuffer.isEmpty()) {
        productNameBuffer = SsuDeviceInfo().displayName(Ssu::DeviceDesignation);
    }
#endif

    if (productNameBuffer.isEmpty()) {
        productNameBuffer = findInRelease(QStringLiteral("PRETTY_NAME")).remove(QStringLiteral("\""));
    }

    if (productNameBuffer.isEmpty()) {
        QProcess lsbRelease;
        lsbRelease.start(QStringLiteral("/usr/bin/lsb_release"),
                         QStringList() << QStringLiteral("-c"));
        if (lsbRelease.waitForFinished()) {
            QString buffer(QString::fromLocal8Bit(lsbRelease.readAllStandardOutput().constData()));
            productNameBuffer = buffer.section(QChar::fromLatin1('\t'), 1, 1).simplified();
        }
    }

    return productNameBuffer;
}

QString QDeviceInfoPrivate::uniqueDeviceID()
{
    if (uniqueDeviceIDBuffer.isEmpty()) {
        // for dmi enabled kernels
        QFile file(QStringLiteral("/sys/devices/virtual/dmi/id/product_uuid"));
        if (file.open(QIODevice::ReadOnly)) {
            QString id = QString::fromLocal8Bit(file.readAll().simplified().data());
            if (id.length() == 36) {
                if (isUuid(id)) {
                    uniqueDeviceIDBuffer = id;
                }
            }
        }
    }

    if (uniqueDeviceIDBuffer.isEmpty()) {
        QFile file(QStringLiteral("/etc/unique-id"));
        if (file.open(QIODevice::ReadOnly)) {
            QString id = QString::fromLocal8Bit(file.readAll().simplified().data());
            if (id.length() == 32) {
                id = id.insert(8,'-').insert(13,'-').insert(18,'-').insert(23,'-');
                if (isUuid(id)) {
                    uniqueDeviceIDBuffer = id;
                }
                file.close();
            }
        }
    }

    if (uniqueDeviceIDBuffer.isEmpty()) { //try wifi mac address
        QNetworkInfo netinfo;
        QString macaddy;
        macaddy = netinfo.macAddress(QNetworkInfo::WlanMode,0);
        if (macaddy.isEmpty())
            macaddy = netinfo.macAddress(QNetworkInfo::EthernetMode,0);
        if (!macaddy.isEmpty()) {
            QCryptographicHash hash2(QCryptographicHash::Sha1);
            hash2.addData(macaddy.toLocal8Bit());

            QUuid id = QUuid::fromRfc4122(hash2.result().left(16));
            if (!id.isNull())
                uniqueDeviceIDBuffer = id.toString();
        }
    }
    if (uniqueDeviceIDBuffer.isEmpty()) {
        QFile file(QStringLiteral("/etc/machine-id"));
        if (file.open(QIODevice::ReadOnly)) {
            QString id = QString::fromLocal8Bit(file.readAll().simplified().data());
            if (id.length() == 32) {
                id = id.insert(8,'-').insert(13,'-').insert(18,'-').insert(23,'-');
                if (isUuid(id)) {
                    uniqueDeviceIDBuffer = id;
                }
            }
            file.close();
        }
    }

//last ditch effort
    if (uniqueDeviceIDBuffer.isEmpty()) {
        QFile file(QStringLiteral("/var/lib/dbus/machine-id"));

        if (file.open(QIODevice::ReadOnly)) {
            QString id = QString::fromLocal8Bit(file.readAll().simplified().data());
            if (id.length() == 32) {
                id = id.insert(8,'-').insert(13,'-').insert(18,'-').insert(23,'-');
                if (isUuid(id)) {
                    uniqueDeviceIDBuffer = id;
                }
            }
            file.close();
        }
    }


    return uniqueDeviceIDBuffer;
}

bool QDeviceInfoPrivate::isUuid(const QString &id)
{
    QUuid uid(id); //make sure this can be made into a valid QUUid
    return !uid.isNull();
}

QString QDeviceInfoPrivate::version(QDeviceInfo::Version type)
{
    switch (type) {
    case QDeviceInfo::Os:

        if (versionBuffer[0].isEmpty()) {
            versionBuffer[0] = findInRelease(QStringLiteral("VERSION_ID"),
                                             QStringLiteral("os-release"));
        }

        if (versionBuffer[0].isEmpty()) {
            versionBuffer[0] = findInRelease(QStringLiteral("VERSION_ID"));
        }

        if (versionBuffer[0].isEmpty() && QFile::exists(QStringLiteral("/usr/bin/lsb_release"))) {
            QProcess lsbRelease;
            lsbRelease.start(QStringLiteral("/usr/bin/lsb_release"),
                             QStringList() << QStringLiteral("-r"));
            if (lsbRelease.waitForFinished()) {
                QString buffer(QString::fromLocal8Bit(lsbRelease.readAllStandardOutput().constData()));
                versionBuffer[0] = buffer.section(QChar::fromLatin1('\t'), 1, 1).simplified();
            }
        }

        return versionBuffer[0];

    case QDeviceInfo::Firmware:
        // Try to read hardware adaptation version first.
        if (versionBuffer[1].isEmpty()) {
            versionBuffer[1] = findInRelease(QStringLiteral("VERSION_ID"),
                                             QStringLiteral("hw-release"));
        }

        if (versionBuffer[1].isEmpty()) {
            QFile file(QStringLiteral("/proc/sys/kernel/osrelease"));
            if (file.open(QIODevice::ReadOnly)) {
                versionBuffer[1] = QString::fromLocal8Bit(file.readAll().simplified().data());
                file.close();
            }
        }
        return versionBuffer[1];
    }

    return QString();
}

QString QDeviceInfoPrivate::operatingSystemName()
{
    if (osName.isEmpty()) {
        osName = findInRelease(QStringLiteral("NAME="), QStringLiteral("os-release"));
    }
    if (osName.isEmpty()) {
        osName = findInRelease(QStringLiteral("NAME="));
    }

    return osName;
}

QString QDeviceInfoPrivate::boardName()
{
    if (boardNameString.isEmpty()) {
        QFile boardfile(QStringLiteral("/etc/boardname"));
        if (boardfile.open(QIODevice::ReadOnly))
            boardNameString = QString::fromLocal8Bit(boardfile.readAll().simplified().data());
    }

    if (boardNameString.isEmpty()) {
        // for dmi enabled kernels
        QFile file(QStringLiteral("/sys/devices/virtual/dmi/id/board_name"));
        if (file.open(QIODevice::ReadOnly))
            boardNameString = QString::fromLocal8Bit(file.readAll().simplified().data());
    }
    return boardNameString;
}

QString QDeviceInfoPrivate::findInRelease(const QString &searchTerm, const QString &file)
{
    QString result;
    QStringList releaseFiles;
    if (file.isEmpty()) {
        releaseFiles = QDir(QStringLiteral("/etc/")).entryList(QStringList() << QStringLiteral("*-release"));
    } else {
        releaseFiles.append(file);
    }

    foreach (const QString &file, releaseFiles) {
        if (!result.isEmpty())
            continue;
        QFile release(QStringLiteral("/etc/") + file);
        if (release.open(QIODevice::ReadOnly))  {
            QTextStream stream(&release);
            QString line;
            do {
                line = stream.readLine();
                if (line.left(searchTerm.size()) == searchTerm) {
                  result = line.split(QStringLiteral("=")).at(1).simplified();
                  // Remove optional quotation marks.
                  result.remove(QRegularExpression(QStringLiteral("^\"|\"$")));
                  break;
                }
            } while (!line.isNull());
            release.close();
        }
    }
    return result;
}

#if !defined(QT_NO_DBUS)
void QDeviceInfoPrivate::bluezPropertyChanged(const QString &str, QDBusVariant v)
{

    if (str == QStringLiteral("Powered")) {
        if (btPowered != v.variant().toBool()) {
            btPowered = !btPowered;
            Q_EMIT bluetoothStateChanged(btPowered);
        }
    } else if (str == QStringLiteral("Adapters")) {
        bool oldPoweredState = btPowered;
        if (oldPoweredState != currentBluetoothPowerState())
            Q_EMIT bluetoothStateChanged(btPowered);
    }
}

void QDeviceInfoPrivate::connectBtPowered()
{
    if (connectedBtPower) {
        QDBusInterface *connectionInterface;
        connectionInterface = new QDBusInterface(QStringLiteral("org.bluez"), QStringLiteral("/"),
                                                 QStringLiteral("org.bluez.Manager"),
                                                 QDBusConnection::systemBus(), this);
        if (connectionInterface->isValid()) {
            QDBusReply <QDBusObjectPath> reply = connectionInterface->call(QStringLiteral("DefaultAdapter"));
            if (reply.isValid() && !reply.value().path().isEmpty()) {
                if (!QDBusConnection::systemBus().connect(QStringLiteral("org.bluez"), reply.value().path(),
                                                          QStringLiteral("org.bluez.Adapter"),
                                                          QStringLiteral("PropertyChanged"),
                                                          this,
                                                          SLOT(bluezPropertyChanged(QString,QDBusVariant)))) {
                }
            }
        }
        connectedBtPower = true;
    }
}
#endif

bool QDeviceInfoPrivate::currentBluetoothPowerState()
{
    bool powered = false;
#ifndef QT_NO_DBUS
    QDBusInterface *connectionInterface = new QDBusInterface(QStringLiteral("org.bluez"),
                                                             QStringLiteral("/"),
                                                             QStringLiteral("org.bluez.Manager"),
                                                             QDBusConnection::systemBus(), this);
    if (connectionInterface->isValid()) {
        QDBusReply<QDBusObjectPath> reply = connectionInterface->call(QStringLiteral("DefaultAdapter"));
        if (reply.isValid() && !reply.value().path().isEmpty()) {
            QDBusInterface *adapterInterface = new QDBusInterface(QStringLiteral("org.bluez"),
                                                                  reply.value().path(),
                                                                  QStringLiteral("org.bluez.Adapter"),
                                                                  QDBusConnection::systemBus(), this);
            if (adapterInterface->isValid()) {
                QDBusReply<QVariantMap> reply =  adapterInterface->call(QStringLiteral("GetProperties"));
                QVariantMap map = reply.value();
                QString property = QStringLiteral("Powered");
                if (map.contains(property))
                    powered =  map.value(property).toBool();
            } else {
                powered = false;
            }
        } else {
            powered = false;
        }
    }
#endif
    return btPowered = powered;
}

extern QMetaMethod proxyToSourceSignal(const QMetaMethod &, QObject *);

void QDeviceInfoPrivate::connectNotify(const QMetaMethod &signal)
{
    if (timer == 0) {
        timer = new QTimer(this);
        timer->setInterval(2000);
        connect(timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    }

    if (!timer->isActive())
        timer->start();

    static const QMetaMethod thermalStateChangedSignal = QMetaMethod::fromSignal(&QDeviceInfoPrivate::thermalStateChanged);
    if (signal == thermalStateChangedSignal) {
        watchThermalState = true;
        currentThermalState = getThermalState();
    }

    static const QMetaMethod bluetoothStateChanged = QMetaMethod::fromSignal(&QDeviceInfoPrivate::bluetoothStateChanged);
    if (signal == bluetoothStateChanged) {
#if !defined(QT_NO_DBUS)
        connectBtPowered();
#endif
    }
}

void QDeviceInfoPrivate::disconnectNotify(const QMetaMethod &signal)
{
    static const QMetaMethod thermalStateChangedSignal = QMetaMethod::fromSignal(&QDeviceInfoPrivate::thermalStateChanged);
    if (signal == thermalStateChangedSignal) {
        watchThermalState = false;
        currentThermalState = QDeviceInfo::UnknownThermal;
    }

    if (!watchThermalState)
        timer->stop();
}

void QDeviceInfoPrivate::onTimeout()
{
    if (watchThermalState) {
        QDeviceInfo::ThermalState newState = getThermalState();
        if (newState != currentThermalState) {
            currentThermalState = newState;
            emit thermalStateChanged(currentThermalState);
        }
    }
}

QDeviceInfo::ThermalState QDeviceInfoPrivate::getThermalState()
{
    QDeviceInfo::ThermalState state = QDeviceInfo::UnknownThermal;

    const QString hwmonRoot(QStringLiteral("/sys/class/hwmon/"));
    const QStringList hwmonDirs(QDir(hwmonRoot).entryList(QStringList() << QStringLiteral("hwmon*")));
    foreach (const QString &dir, hwmonDirs) {
        int index = 1;
        const QString input(hwmonRoot + dir + QDir::separator() + QStringLiteral("temp%1_input"));
        const QString critical(hwmonRoot + dir + QDir::separator() + QStringLiteral("temp%1_crit"));
        const QString emergency(hwmonRoot + dir + QDir::separator() + QStringLiteral("temp%1_emergency"));
        while (true) {
            QFile file(input.arg(index));
            if (!file.open(QIODevice::ReadOnly))
                break;
            bool ok(false);
            int currentTemp = file.readAll().simplified().toInt(&ok);
            if (ok) {
                if (state == QDeviceInfo::UnknownThermal)
                    state = QDeviceInfo::NormalThermal;

                // Only check if we are below WarningThermal
                if (state < QDeviceInfo::WarningThermal) {
                    file.close();
                    file.setFileName(critical.arg(index));
                    if (file.open(QIODevice::ReadOnly)) {
                        int criticalTemp = file.readAll().simplified().toInt(&ok);
                        if (ok && currentTemp > criticalTemp)
                            state = QDeviceInfo::WarningThermal;
                    }
                }

                // Only check if we are below AlertThermal
                if (state < QDeviceInfo::AlertThermal) {
                    file.close();
                    file.setFileName(emergency.arg(index));
                    if (file.open(QIODevice::ReadOnly)) {
                        int emergencyTemp = file.readAll().simplified().toInt(&ok);
                        if (ok && currentTemp > emergencyTemp) {
                            state = QDeviceInfo::AlertThermal;
                            break; // No need for further checking, as we can't get the ErrorThermal state
                        }
                    }
                }
            } else {
                break;
            }

            ++index;
        }
    }

    return state;
}

QT_END_NAMESPACE
