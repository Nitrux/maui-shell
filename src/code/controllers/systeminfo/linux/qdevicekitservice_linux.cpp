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

#include "qdevicekitservice_linux_p.h"

QT_BEGIN_NAMESPACE

#define UPOWER_SERVICE        "org.freedesktop.UPower"
#define UPOWER_PATH           "/org/freedesktop/UPower"
#define UPOWER_DEVICE_SERVICE "org.freedesktop.UPower.Device"
#define UPOWER_DEVICE_PATH    "/org/freedesktop/UPower/Device"

QUPowerInterface::QUPowerInterface(QObject *parent)
    : QDBusAbstractInterface(QLatin1String(UPOWER_SERVICE)
    , QLatin1String(UPOWER_PATH)
    , UPOWER_SERVICE
    , QDBusConnection::systemBus()
    , parent)
{
}

QUPowerInterface::~QUPowerInterface()
{
}

QList<QDBusObjectPath> QUPowerInterface::enumerateDevices()
{
    QDBusPendingReply<QList<QDBusObjectPath> > reply = call(QLatin1String("EnumerateDevices"));
    reply.waitForFinished();
    if (reply.isError())
        qDebug() << reply.error().message();
    return reply.value();
}

void QUPowerInterface::connectNotify(const QMetaMethod &signal)
{
    static const QMetaMethod addedSignal = QMetaMethod::fromSignal(&QUPowerInterface::deviceAdded);
    static const QMetaMethod removedSignal = QMetaMethod::fromSignal(&QUPowerInterface::deviceRemoved);

    if (signal == addedSignal) {
        if (!connection().connect(QStringLiteral(UPOWER_SERVICE),
                                  QStringLiteral(UPOWER_PATH),
                                  QStringLiteral(UPOWER_SERVICE),
                                  QStringLiteral("DeviceAdded"),
                                  this, SIGNAL(onDeviceAdded(QDBusObjectPath)))) {
            qDebug() << "Error"<<connection().lastError().message();
        }

    }
    if (signal == removedSignal) {
        if (!connection().connect(QStringLiteral(UPOWER_SERVICE),
                                  QStringLiteral(UPOWER_PATH),
                                  QStringLiteral(UPOWER_SERVICE),
                                  QStringLiteral("DeviceRemoved"),
                                  this, SIGNAL(onDeviceRemoved(QDBusObjectPath)))) {
            qDebug() << "Error"<<connection().lastError().message();
        }
    }
}

void QUPowerInterface::disconnectNotify(const QMetaMethod &signal)
{
    static const QMetaMethod addedSignal = QMetaMethod::fromSignal(&QUPowerInterface::deviceAdded);
    static const QMetaMethod removedSignal = QMetaMethod::fromSignal(&QUPowerInterface::deviceRemoved);

    if (signal == addedSignal) {
        if (!connection().disconnect(QStringLiteral(UPOWER_SERVICE),
                                  QStringLiteral(UPOWER_PATH),
                                  QStringLiteral(UPOWER_SERVICE),
                                  QStringLiteral("DeviceAdded"),
                                  this, SLOT(onDeviceAdded(QDBusObjectPath)))) {
            qDebug() << "Error"<<connection().lastError().message();
        }

    }
    if (signal == removedSignal) {
        if (!connection().disconnect(QStringLiteral(UPOWER_SERVICE),
                                  QStringLiteral(UPOWER_PATH),
                                  QStringLiteral(UPOWER_SERVICE),
                                  QStringLiteral("DeviceRemoved"),
                                  this, SLOT(onDeviceRemoved(QDBusObjectPath)))) {
            qDebug() << "Error"<<connection().lastError().message();
        }
    }

}

void QUPowerInterface::onDeviceAdded(const QDBusObjectPath &path)
{
    Q_EMIT deviceAdded(path.path());
}

void QUPowerInterface::onDeviceRemoved(const QDBusObjectPath &path)
{
    Q_EMIT deviceRemoved(path.path());
}

QUPowerDeviceInterface::QUPowerDeviceInterface(const QString &dbusPathName, QObject *parent)
    : QDBusAbstractInterface(QLatin1String(UPOWER_SERVICE)
    , dbusPathName
    , UPOWER_DEVICE_SERVICE
    , QDBusConnection::systemBus()
    , parent)
{
    // We make a synchronous properties request here so we have a valid state
    QDBusMessage propGetMsg = QDBusMessage::createMethodCall(UPOWER_SERVICE, path(), QStringLiteral("org.freedesktop.DBus.Properties"), QLatin1String("GetAll"));
    QList<QVariant> arguments;
    arguments << QLatin1String("org.freedesktop.UPower.Device");
    propGetMsg.setArguments(arguments);
    QDBusReply<QVariantMap> propReply = QDBusConnection::systemBus().call(propGetMsg);

    if (!propReply.isValid()) {
        qWarning() << "QUPowerDeviceInterface: error getting properties: " << propReply.error();
        return;
    }

    pMap = propReply.value();
}

QUPowerDeviceInterface::~QUPowerDeviceInterface()
{
}

quint16 QUPowerDeviceInterface::type()
{
    return pMap.value(QStringLiteral("Type")).toUInt();
}

bool QUPowerDeviceInterface::isPowerSupply()
{
    return pMap.value(QStringLiteral("PowerSupply")).toBool();
}

bool QUPowerDeviceInterface::isOnline()
{
    return pMap.value(QStringLiteral("Online")).toBool();
}

double QUPowerDeviceInterface::currentEnergy()
{
    return pMap.value(QStringLiteral("Energy")).toDouble();
}

double QUPowerDeviceInterface::energyWhenFull()
{
    return pMap.value(QStringLiteral("EnergyFull")).toDouble();
}

double QUPowerDeviceInterface::energyDischargeRate()
{
    return pMap.value(QStringLiteral("EnergyRate")).toDouble();
}

double QUPowerDeviceInterface::voltage()
{
    return pMap.value(QStringLiteral("Voltage")).toDouble();
}

qint64 QUPowerDeviceInterface::timeToFull()
{
    return pMap.value(QStringLiteral("TimeToFull")).toUInt();
}

double QUPowerDeviceInterface::percentLeft()
{
    return pMap.value(QStringLiteral("Percentage")).toDouble();
}

quint16 QUPowerDeviceInterface::state()
{
    return pMap.value(QStringLiteral("State")).toUInt();
}

void QUPowerDeviceInterface::connectNotify(const QMetaMethod &signal)
{
    static const QMetaMethod changedSignal = QMetaMethod::fromSignal(&QUPowerDeviceInterface::changed);
    static const QMetaMethod propertyChangedSignal = QMetaMethod::fromSignal(&QUPowerDeviceInterface::propertyChanged);

    if (signal == changedSignal) {
        if (!connection().connect(QLatin1String(UPOWER_SERVICE),
                                  path(),
                                  QLatin1String(UPOWER_DEVICE_SERVICE),
                                  QLatin1String("Changed"),
                                  this, SIGNAL(changed()))) {
            qDebug() << "Error" << connection().lastError().message();
        }
    }
    if (signal == propertyChangedSignal) {
        if (!connection().connect(QLatin1String(UPOWER_SERVICE),
                                  path(),
                                  QLatin1String(UPOWER_DEVICE_SERVICE),
                                  QLatin1String("Changed"),
                                  this, SIGNAL(propChanged()))) {
            qDebug() << "Error" << connection().lastError().message();
        }
    }
}

void QUPowerDeviceInterface::disconnectNotify(const QMetaMethod &signal)
{
    static const QMetaMethod changedSignal = QMetaMethod::fromSignal(&QUPowerDeviceInterface::changed);
    static const QMetaMethod propertyChangedSignal = QMetaMethod::fromSignal(&QUPowerDeviceInterface::propertyChanged);
    if (signal == changedSignal) {
        connection().disconnect(QLatin1String(UPOWER_SERVICE),
                                path(),
                                QLatin1String(UPOWER_DEVICE_SERVICE),
                                QLatin1String("Changed"),
                                this, SIGNAL(changed()));
    }
    if (signal == propertyChangedSignal) {
        connection().disconnect(QLatin1String(UPOWER_SERVICE),
                                path(),
                                QLatin1String(UPOWER_DEVICE_SERVICE),
                                QLatin1String("Changed"),
                                this, SIGNAL(propChanged()));
    }
}

void QUPowerDeviceInterface::propChanged()
{
    QDBusMessage propGetMsg = QDBusMessage::createMethodCall(UPOWER_SERVICE, path(), QStringLiteral("org.freedesktop.DBus.Properties"), QLatin1String("GetAll"));
    QList<QVariant> arguments;
    arguments << QLatin1String("org.freedesktop.UPower.Device");
    propGetMsg.setArguments(arguments);
    QDBusPendingCall asyncPropReply = QDBusConnection::systemBus().asyncCall(propGetMsg);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(asyncPropReply, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, &QUPowerDeviceInterface::propRequestFinished);
}

void QUPowerDeviceInterface::propRequestFinished(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<QVariantMap> reply = *call;
    call->deleteLater();

    if (!reply.isValid()) {
        // don't throw away the existing map if the call fails
        qWarning() << "QUPowerDeviceInterface: fetching properties failed: " << reply.error();
        return;
    }

    QVariantMap map = pMap; // copy to compare
    pMap = reply.value();
    QMapIterator<QString, QVariant> i(reply.value());

    while (i.hasNext()) {
        i.next();
        if (pMap.value(i.key()) != map.value(i.key())) {
            Q_EMIT propertyChanged(i.key(), QVariant::fromValue(i.value()));
        }
    }
}

QString QUPowerDeviceInterface::nativePath()
{
    return pMap.value(QStringLiteral("NativePath")).toString();
}

quint16 QUPowerDeviceInterface::technology()
{
    return pMap.value(QStringLiteral("Technology")).toUInt();
}

QT_END_NAMESPACE
