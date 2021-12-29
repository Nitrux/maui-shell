/****************************************************************************
**
** Copyright (C) 2018 Canonical, Ltd. and/or its subsidiary(-ies).
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

#include "qinputinfo.h"

#include "qinputinfomanager_p.h"

Q_GLOBAL_STATIC(QInputInfoManagerPrivate, inputDeviceManagerPrivate)

#if defined(Q_OS_LINUX)
#if !defined(QT_NO_UDEV)
#include "linux/qinputinfomanagerudev_p.h"
#endif

#if !defined(QT_NO_MIR)
#include <QGuiApplication>
#include <qpa/qplatformnativeinterface.h>
#include "linux/qinputinfomanagermir_p.h"
#endif

#if !defined(QT_NO_UDEV)
Q_GLOBAL_STATIC(QInputInfoManagerUdev, inputDeviceManagerUdev)
#endif
#if !defined(QT_NO_MIR)
static void not_owning_connection(MirConnection*) {}
QInputInfoManagerMir::MirConnectionPtr attempt_to_connect_to_mir()
{
    auto nativeInterface = QGuiApplication::platformNativeInterface();

    if (nativeInterface) {
        auto connection = static_cast<MirConnection*>(nativeInterface->nativeResourceForIntegration("mirconnection"));
        return QInputInfoManagerMir::MirConnectionPtr{connection, &not_owning_connection};
    } else {
        auto connection = mir_connect_sync(NULL, "qtsystems-plugin");
        if (mir_connection_is_valid(connection))
            return QInputInfoManagerMir::MirConnectionPtr{connection, &mir_connection_release};
        else
            return QInputInfoManagerMir::MirConnectionPtr{connection, &not_owning_connection};
    }
}

Q_GLOBAL_STATIC_WITH_ARGS(QInputInfoManagerMir, inputDeviceManagerMir, (attempt_to_connect_to_mir()))
#endif
#endif

QT_BEGIN_NAMESPACE

/*!
    \internal
*/
QInputInfoManagerPrivate * QInputInfoManagerPrivate::instance()
{
#ifndef QT_NO_MIR
    if (inputDeviceManagerMir.exists()) {
        return inputDeviceManagerMir();
    } else {
        auto connection_attempt = attempt_to_connect_to_mir();
        if (connection_attempt && mir_connection_is_valid(connection_attempt.get())) {
            connection_attempt.reset();
            return inputDeviceManagerMir();
        }
    }
#endif
#ifndef QT_NO_UDEV
    return inputDeviceManagerUdev();
#endif
    return inputDeviceManagerPrivate();
}

/*!
    \internal
*/
QInputDevicePrivate::QInputDevicePrivate(QObject *parent) :
    QObject(parent),
    type(QInputDevice::UnknownType)
{
    qRegisterMetaType<QInputDevice::InputType>();
    qRegisterMetaType<QInputDevice::InputTypeFlags>();
}


/*!
    \class QInputDevice
    \inmodule QtSystemInfo
    \brief The QInputDevice class provides various information about one input device.
    \ingroup systeminfo

*/
/*!
    \enum InputType::InputType
    This enum describes the type of input.

    \value UnknownType         The input type is unknown.
    \value Button              Input is a button.
    \value Mouse               Input is a mouse.
    \value TouchPad            Input is a touch pad.
    \value TouchScreen         Input is a touch screen.
    \value Keyboard            Input is a keyboard
    \value Switch              Input is a switch
*/

/*!
    Constructs a \l QInputDevice object with the given \a parent.
*/
QInputDevice::QInputDevice(QObject *parent) :
    QObject(parent),
    d_ptr(new QInputDevicePrivate(this))
{
}

/*!
    \property QInputDevice::properties
    \brief The properties of this input device.

    This property contains a variant map of all properties known for this input device.
    Name and identifier properties are required.

  \sa name(), identifier(), buttons(), switches(), relativeAxes(), absoluteAxes(), type()

*/
QVariantMap QInputDevice::properties()
{
    return deviceProperties;
}

/*
    Returns the name of this input device.
 */
QString QInputDevice::name() const
{
    return d_ptr->name;
}

/*
    Sets the name of this input device to \b name.
 */
void QInputDevice::setName(const QString &name)
{
    deviceProperties.insert(QStringLiteral("name"),name);
    d_ptr->name = name;
}

/*
    Returns the identifier of this device.
 */
QString QInputDevice::identifier() const
{
    return d_ptr->identifier;
}

/*
    Sets the identifier of this device to /b id.
 */
void QInputDevice::setIdentifier(const QString &id)
{
    d_ptr->identifier = id;
    deviceProperties.insert(QStringLiteral("identifier"),id);
}

/*
    Returns the number of buttons this device has.
 */
QList <int> QInputDevice::buttons() const
{
    return d_ptr->buttons;
}

/*
    \internal
 */
void QInputDevice::addButton(int buttonCode)
{
    d_ptr->buttons.append(buttonCode);
    deviceProperties.insert(QStringLiteral("buttons"),QVariant::fromValue(d_ptr->buttons));
}

/*
    Returns the number of switches of this device.
 */
QList <int> QInputDevice::switches() const
{
    return d_ptr->switches;
}

/*
    \internal
 */
void QInputDevice::addSwitch(int switchCode)
{
    d_ptr->switches.append(switchCode);
    deviceProperties.insert(QStringLiteral("switches"),QVariant::fromValue(d_ptr->switches));
}

/*
    Returns a list of the relative axes of this device
 */
QList <int> QInputDevice::relativeAxes() const
{
    return d_ptr->relativeAxes;
}

/*
    \internal
 */
void QInputDevice::addRelativeAxis(int axisCode)
{
    d_ptr->relativeAxes.append(axisCode);
    deviceProperties.insert(QStringLiteral("rAxis"),QVariant::fromValue(d_ptr->relativeAxes));
}

/*
    Returns a list of the absolute axes of this device
 */
QList <int> QInputDevice::absoluteAxes() const
{
    return d_ptr->absoluteAxes;
}

/*
    \internal
 */
void QInputDevice::addAbsoluteAxis(int axisCode)
{
    d_ptr->absoluteAxes.append(axisCode);
    deviceProperties.insert(QStringLiteral("aAxis"),QVariant::fromValue(d_ptr->absoluteAxes));
}

/*
    Returns a QInputDevice::InputTypeFlags of all the types of types.
 */
QInputDevice::InputTypeFlags QInputDevice::types() const
{
    return d_ptr->type;
}

/*
    \internal
 */
void QInputDevice::setTypes(QInputDevice::InputTypeFlags type)
{
    d_ptr->type = type;
    deviceProperties.insert(QStringLiteral("types"),QVariant::fromValue(type));
}

/*!
    \class QInputInfoManager
    \inmodule QtSystemInfo

    \brief The QInputInfoManager class manages input devices.

*/
QInputInfoManager::QInputInfoManager(QObject *parent) :
    QObject(parent),
    d_ptr(QInputInfoManagerPrivate::instance()),
    currentFilter(QInputDevice::Button | QInputDevice::Mouse
                   | QInputDevice::TouchPad | QInputDevice::TouchScreen
                   | QInputDevice::Keyboard
                   | QInputDevice::Switch),
   filteredCount(0)
{

    connect(d_ptr, &QInputInfoManagerPrivate::deviceAdded,this,&QInputInfoManager::addedDevice);
    connect(d_ptr, &QInputInfoManagerPrivate::deviceRemoved,this,&QInputInfoManager::removedDevice);

    connect(d_ptr,SIGNAL(ready()),this,SLOT(privateReady()));

}

/*!
    Destroy the QSensorGestureManager
*/
QInputInfoManager::~QInputInfoManager()
{
}
/*!
    \fn QInputInfoManager::ready()

    This signal is emitted when input device map and associated properties are ready to use.
*/
/*!
    \fn QInputInfoManager::deviceAdded(QInputDevice *inputDevice)

    This signal is emitted when a new input device is added.
*/
/*!
    \fn QInputInfoManager::deviceRemoved(const QString &deviceId)

    This signal is emitted when a new input device is removed.
*/

/*
    \internal
 */
void QInputInfoManager::privateReady()
{
    setFilter(currentFilter);
    Q_EMIT ready();
}

/*
    Returns a QMap of input devices using the currently set QInputDevice::InputTypeFlags filter.
 */
QMap <QString, QInputDevice *> QInputInfoManager::deviceMap()
{
    return currentFilteredMap;
}

/*
    \internal
 */
void QInputInfoManager::addedDevice(QInputDevice *deviceInfo)
{
    Q_UNUSED(deviceInfo);
    int oldFilteredCount = filteredCount;

    setFilter(currentFilter);

    if (oldFilteredCount < filteredCount) {
        Q_EMIT deviceAdded(deviceInfo);
    }
}

/*
    \internal
 */
void QInputInfoManager::removedDevice(const QString &deviceId)
{
    bool ok = false;
    if (currentFilteredMap.contains(deviceId))
        ok = true;
    setFilter(currentFilter);
    if (ok) {
        Q_EMIT deviceRemoved(deviceId);
    }
}

/*
    Returns the number of input devices with the currently set QInputDevice::InputTypeFlags filter,
    if no filter is set, this returns number of all available input devices.
 */
int QInputInfoManager::count() const
{
    int deviceCount = 0;
    if (currentFilter.testFlag(QInputDevice::Button)) {
        deviceCount += count(static_cast< QInputDevice::InputType >(QInputDevice::Button));
    }
    if (currentFilter.testFlag(QInputDevice::Mouse)) {
        deviceCount += count(static_cast< QInputDevice::InputType >(QInputDevice::Mouse));
    }
    if (currentFilter.testFlag(QInputDevice::TouchPad)) {
        deviceCount += count(static_cast< QInputDevice::InputType >(QInputDevice::TouchPad));
    }
    if (currentFilter.testFlag(QInputDevice::TouchScreen)) {
        deviceCount += count(static_cast< QInputDevice::InputType >(QInputDevice::TouchScreen));
    }
    if (currentFilter.testFlag(QInputDevice::Keyboard)) {
        deviceCount += count(static_cast< QInputDevice::InputType >(QInputDevice::Keyboard));
    }
    if (currentFilter.testFlag(QInputDevice::Switch)) {
        deviceCount += count(static_cast< QInputDevice::InputType >(QInputDevice::Switch));
    }
    return deviceCount;
}

/*
    \property QInputInfoManager::count
    \brief The number of input devices of the type filter.

    Returns the number of input devices of the type \a filter, regardless of the filter
    that is currently set.
 */
/*!
    \fn QInputInfoManager::countChanged(int count)

    This signal is emitted when the count of devices in regards to the current filter changes.
*/

int QInputInfoManager::count(const QInputDevice::InputType filter) const
{
    int dList = 0;
    QMapIterator<QString, QInputDevice *> i(d_ptr->deviceMap);
    while (i.hasNext()) {
        i.next();
        if (i.value()->types().testFlag(filter)) {
            dList++;
        }
    }
    return dList;
}

/*
    \property QInputInfoManager::filter
    \brief The currently set filter.

    Returns the currently set device filter.
    If none is set, default filter includes all QInputDevice::InputType
 */

/*!
    \fn QInputInfoManager::filterChanged(QInputDevice::InputTypeFlags filterFlags)

    This signal is emitted when the filter property changes.
*/

QInputDevice::InputTypeFlags QInputInfoManager::filter()
{
    return currentFilter;
}

/*
 * Sets the current input device filter to \a filter, and updates device map.
 *
 * \sa deviceMap()
 */
void QInputInfoManager::setFilter(QInputDevice::InputTypeFlags filter)
{
    currentFilteredMap.clear();
    if (!filter) {
        filter = filter & (QInputDevice::Button | QInputDevice::Mouse
                | QInputDevice::TouchPad | QInputDevice::TouchScreen
                | QInputDevice::Keyboard | QInputDevice::Switch);
    }
    QMapIterator<QString, QInputDevice *> i(d_ptr->deviceMap);
    while (i.hasNext()) {
        i.next();

        if (filter.testFlag(QInputDevice::Button)
                && i.value()->types().testFlag(QInputDevice::Button)) {
            currentFilteredMap.insert(i.key(),i.value());
        }
        if (filter.testFlag(QInputDevice::Mouse)
                && i.value()->types().testFlag(QInputDevice::Mouse)) {
            currentFilteredMap.insert(i.key(),i.value());
        }
        if (filter.testFlag(QInputDevice::TouchPad)
                && i.value()->types().testFlag(QInputDevice::TouchPad)) {
            currentFilteredMap.insert(i.key(),i.value());
        }
        if (filter.testFlag(QInputDevice::TouchScreen)
                && i.value()->types().testFlag(QInputDevice::TouchScreen)) {
            currentFilteredMap.insert(i.key(),i.value());
        }
        if (filter.testFlag(QInputDevice::Keyboard)
                && i.value()->types().testFlag(QInputDevice::Keyboard)) {
            currentFilteredMap.insert(i.key(),i.value());
        }
        if (filter.testFlag(QInputDevice::Switch)
                && i.value()->types().testFlag(QInputDevice::Switch)) {
            currentFilteredMap.insert(i.key(),i.value());
        }
    }

    if (filter != currentFilter) {
        currentFilter = filter;
        Q_EMIT filterChanged(filter);
    }

    if (currentFilteredMap.count() != filteredCount) {
        filteredCount = currentFilteredMap.count();
        Q_EMIT countChanged(filteredCount);
    }
}

/*
    \property QInputInfoManager::lastAdded
    \brief The last added input device.

    Returns the last added input device.
 */
QInputDevice *QInputInfoManager::lastAdded()
{
   return currentFilteredMap.last();
}

QT_END_NAMESPACE
