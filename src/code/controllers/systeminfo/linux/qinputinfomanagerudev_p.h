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

#ifndef QINPUTDEVICEMANAGER_UDEV_P_H
#define QINPUTDEVICEMANAGER_UDEV_P_H

#include "qinputinfomanager_p.h"
#include <libudev.h>

QT_BEGIN_NAMESPACE

class QInputInfoManagerUdev : public QInputInfoManagerPrivate
{
    Q_OBJECT
public:
    explicit QInputInfoManagerUdev(QObject *parent = 0);
    ~QInputInfoManagerUdev();

private:
    QInputDevice *addDevice(struct udev_device *udev);
    QInputDevice *addUdevDevice(struct udev_device *);

    QInputDevice *addDevice(const QString &path);
    void removeDevice(const QString &path);
    QSocketNotifier *notifier;
    int notifierFd;
    struct udev_monitor *udevMonitor;
    QInputDevice::InputTypeFlags getInputTypeFlags(struct udev_device *);
    struct udev *udevice;
    void addDetails(struct udev_device *);

private Q_SLOTS:
    void onUDevChanges();
    void init();
};

QT_END_NAMESPACE

#endif // QINPUTDEVICEMANAGER_UDEV_P_H
