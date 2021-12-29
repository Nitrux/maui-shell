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

#include "qinputinfomanagerudev_p.h"

#include <libudev.h>
#ifndef QT_NO_EVDEV
#include <libevdev/libevdev.h>
#endif
#include <fcntl.h>
#include <QDebug>
#include <QDir>
#include <QSocketNotifier>
#include <QTimer>

QInputInfoManagerUdev::QInputInfoManagerUdev(QObject *parent) :
    QInputInfoManagerPrivate(parent),
    udevice(0)
{
    QTimer::singleShot(250,this,SLOT(init()));
}

QInputInfoManagerUdev::~QInputInfoManagerUdev()
{
    udev_unref(udevice);
    udev_monitor_unref(udevMonitor);
}

void QInputInfoManagerUdev::init()
{
    if (!udevice)
        udevice = udev_new();

    udev_list_entry *devices;
    udev_list_entry *dev_list_entry;
    udev_device *dev;

    QString subsystem = QStringLiteral("input");
    struct udev_enumerate *enumerate = 0;

    if (udevice) {

        udevMonitor = udev_monitor_new_from_netlink(udevice, "udev");
        if (udevMonitor) {
            udev_monitor_filter_add_match_subsystem_devtype(udevMonitor, subsystem.toLatin1(), NULL);
            enumerate = udev_enumerate_new(udevice);
            udev_enumerate_add_match_subsystem(enumerate, subsystem.toLatin1());

            udev_monitor_enable_receiving(udevMonitor);
            notifierFd = udev_monitor_get_fd(udevMonitor);

            notifier = new QSocketNotifier(notifierFd, QSocketNotifier::Read, this);
            connect(notifier, SIGNAL(activated(int)), this, SLOT(onUDevChanges()));

            udev_enumerate_scan_devices(enumerate);
            devices = udev_enumerate_get_list_entry(enumerate);

            udev_list_entry_foreach(dev_list_entry, devices) {
                const char *path;
                path = udev_list_entry_get_name(dev_list_entry);

                dev = udev_device_new_from_syspath(udevice, path);
                if (qstrcmp(udev_device_get_subsystem(dev), "input") == 0 ) {
                    QInputDevice *iDevice = addDevice(dev);
                    if (iDevice && !iDevice->identifier().isEmpty()) {
                        deviceMap.insert(iDevice->identifier(),iDevice);
                    }
                }
                udev_device_unref(dev);
            }
            udev_enumerate_unref(enumerate);
        }
    }
 //   udev_unref(udevice);
    Q_FOREACH (const QString &devicePath, deviceMap.keys()) {
        Q_EMIT deviceAdded(deviceMap.value(devicePath));
    }
    Q_EMIT ready();
}

QInputDevice::InputTypeFlags QInputInfoManagerUdev::getInputTypeFlags(struct udev_device *dev)
{
    QInputDevice::InputTypeFlags flags = QInputDevice::UnknownType;
    if (qstrcmp(udev_device_get_property_value(dev, "ID_INPUT_KEY"), "1") == 0 ) {
        flags |= QInputDevice::Button;
    }
    if (qstrcmp(udev_device_get_property_value(dev, "ID_INPUT_MOUSE"), "1") == 0) {
        flags |= QInputDevice::Mouse;
    }
    if (qstrcmp(udev_device_get_property_value(dev, "ID_INPUT_TOUCHPAD"), "1") == 0) {
        flags |= QInputDevice::TouchPad;
    }
    if (qstrcmp(udev_device_get_property_value(dev, "ID_INPUT_TOUCHSCREEN"), "1") == 0
            || qstrcmp(udev_device_get_property_value(dev, "ID_INPUT_TABLET"), "1") == 0) {
        flags |= QInputDevice::TouchScreen;
    }
    if (qstrcmp(udev_device_get_property_value(dev, "ID_INPUT_KEYBOARD"), "1") == 0 ) {
        flags |= QInputDevice::Keyboard;
    }
    if (!QString::fromLatin1(udev_device_get_property_value(dev, "SW")).isEmpty()) {
        flags |= QInputDevice::Switch;
    }

    return flags;
}

QInputDevice *QInputInfoManagerUdev::addDevice(struct udev_device *udev)
{
    QString eventPath = QString::fromLatin1(udev_device_get_sysname(udev));

    if (eventPath.contains(QStringLiteral("event")))
        eventPath.prepend(QStringLiteral("/dev/input/"));

    if (deviceMap.contains(eventPath)) {
        return Q_NULLPTR;
    }
    QInputDevice *inputDevice;
    inputDevice = addUdevDevice(udev);
    if (!inputDevice) {
        return Q_NULLPTR;
    }
    eventPath = inputDevice->identifier();

    int fd = open(eventPath.toLatin1(), O_RDONLY|O_NONBLOCK);
    if (fd == -1) {
        return inputDevice;
    }
#ifndef QT_NO_EVDEV
    struct libevdev *dev = NULL;
    int rc = 1;
    rc = libevdev_new_from_fd(fd, &dev);
    if (rc < 0) {
        qWarning() << "Failed to init libevdev ("<< strerror(-rc) << ")";
        return Q_NULLPTR;
    }

    for (int i = 0; i < EV_MAX; i++) {
        if (i == EV_KEY || i == EV_SW || i == EV_REL
                || i == EV_REL || i == EV_ABS) {
            for (int j = 0; j <  libevdev_event_type_get_max(i); j++) {
                if (libevdev_has_event_code(dev, i, j)) {
                    switch (i) {
                    case EV_KEY:
                        inputDevice->addButton(j);
                        break;
                    case EV_SW:
                        inputDevice->addSwitch(j);
                        break;
                    case EV_REL:
                        inputDevice->addRelativeAxis(j);
                        break;
                    case EV_ABS:
                        inputDevice->addAbsoluteAxis(j);
                        break;
                    };
                }
            }
        }
    }
#endif
    return inputDevice;
}

void QInputInfoManagerUdev::addDetails(struct udev_device *)
{
}

void QInputInfoManagerUdev::removeDevice(const QString &path)
{
    // this path is not a full evdev path
    Q_FOREACH (const QString devicePath, deviceMap.keys()) {
        if (devicePath.contains(path)) {
            QInputDevice *removedDevice = deviceMap.take(devicePath);
            removedDevice->deleteLater();
            Q_EMIT deviceRemoved(devicePath);
        }
    }
}

QInputDevice *QInputInfoManagerUdev::addUdevDevice(struct udev_device *udev)
{
    QInputDevice *iDevice;

    struct udev_list_entry *list;
    struct udev_list_entry *node;

    list = udev_device_get_properties_list_entry (udev);
    QString syspath = QString::fromLatin1(udev_device_get_syspath(udev));
    QDir sysdir(syspath);

    QStringList infoList = sysdir.entryList(QStringList() << QStringLiteral("event*"),QDir::Dirs);

    if (infoList.count() > 0) {
        QString token = infoList.at(0);

        token.prepend(QStringLiteral("/dev/input/"));
        iDevice = new QInputDevice(this);
        iDevice->setIdentifier(token);
    } else {
        return Q_NULLPTR;
    }
    udev_list_entry_foreach (node, list) {

        QString key = QString::fromLatin1(udev_list_entry_get_name(node));
        QString value = QString::fromLatin1(udev_list_entry_get_value(node));

        if (key == QStringLiteral("NAME")) {
            iDevice->setName(value.remove(QStringLiteral("\"")));
        }
    }
    iDevice->setTypes(getInputTypeFlags(udev));
    return iDevice;
}

void QInputInfoManagerUdev::onUDevChanges()
{
    if (!udevMonitor)
        return;

    udev_device *dev = udev_monitor_receive_device(udevMonitor);

    if (dev) {
        if (qstrcmp(udev_device_get_subsystem(dev), "input") == 0 ) {
            QString eventPath = QString::fromLatin1(udev_device_get_sysname(dev));

            QString action = QString::fromStdString(udev_device_get_action(dev));

            if (!eventPath.contains(QStringLiteral("/dev/input/")))
                eventPath.prepend(QStringLiteral("/dev/input/"));

            if (action == QStringLiteral("add")) {
                if (deviceMap.contains(eventPath)){
                    udev_device_unref(dev);
                    return;
                }

                QInputDevice *iDevice = addDevice(dev);
                if (!iDevice) {
                    delete iDevice;
                    return;
                }
                iDevice->setTypes(getInputTypeFlags(dev));
                udev_device_unref(dev);
                deviceMap.insert(eventPath,iDevice);
                Q_EMIT deviceAdded(deviceMap.value(eventPath));

            } else if (action == QStringLiteral("remove")) {
                removeDevice(eventPath);
            }
        }
    }
}
