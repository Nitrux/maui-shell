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

#include "qinputinfomanagermir_p.h"
#include "linux/input-event-codes.h"
#include <QTimer>

static void no_input_config_yet(MirInputConfig const*) {}

QInputInfoManagerMir::QInputInfoManagerMir(MirConnectionPtr con, QObject *parent)
    : QInputInfoManagerPrivate(parent), connection{std::move(con)}, config{nullptr, &no_input_config_yet}
{
    if (connection && mir_connection_is_valid(connection.get()))
        QTimer::singleShot(250,this,SLOT(init()));
}

QInputInfoManagerMir::~QInputInfoManagerMir()
{
}

void QInputInfoManagerMir::init()
{
    config = MirInputConfigPtr{mir_connection_create_input_config(connection.get()), mir_input_config_destroy};

    mir_connection_set_input_config_change_callback(
        connection.get(),
        [](MirConnection* con, void* context) {
            QInputInfoManagerMir *this_ = static_cast<QInputInfoManagerMir*>(context);
            this_->config = MirInputConfigPtr{mir_connection_create_input_config(con), mir_input_config_destroy};
            this_->update_devices();
        }, this);

    update_devices();
}

void QInputInfoManagerMir::update_devices()
{
    QList<QString> deletedDevices = deviceMap.keys();

    for (int index = 0, e = mir_input_config_device_count(config.get()); index!=e; ++index) {

        MirInputDevice const *input_device = mir_input_config_get_device(
            config.get(),
            index);

        QString id = QString::number(mir_input_device_get_id(input_device));
        if (!deviceMap.contains(id)) {
            QInputDevice *nDevice = new QInputDevice(this);
            nDevice->setName(QString::fromUtf8(mir_input_device_get_name(input_device)));
            nDevice->setIdentifier(id);
            QInputDevice::InputTypeFlags flags = QInputDevice::UnknownType;

            MirInputDeviceCapabilities caps = mir_input_device_get_capabilities(input_device);

            if (caps & mir_input_device_capability_pointer) {
                nDevice->addRelativeAxis(REL_X);
                nDevice->addRelativeAxis(REL_Y);
                nDevice->addRelativeAxis(REL_WHEEL);
                nDevice->addButton(BTN_MOUSE);
                nDevice->addButton(BTN_RIGHT);
                nDevice->addButton(BTN_MIDDLE);
                if (caps & mir_input_device_capability_touchpad) {
                    flags |= QInputDevice::TouchPad;
                } else {
                    flags |= QInputDevice::Mouse;
                    nDevice->addButton(BTN_SIDE);
                    nDevice->addButton(BTN_EXTRA);
                    nDevice->addButton(BTN_FORWARD);
                    nDevice->addButton(BTN_BACK);
                    nDevice->addButton(BTN_TASK);
                }
            }

            if (caps & mir_input_device_capability_keyboard) {
                flags |= QInputDevice::Button;

                // keyboard with enough keys for text entry
                if (caps & mir_input_device_capability_alpha_numeric) {
                    flags |= QInputDevice::Keyboard;
                    for (int i = KEY_1; i != KEY_SLASH; ++i)
                        nDevice->addButton(i);
                }
            }

            nDevice->setTypes(flags);

            deviceMap.insert(id, nDevice);
            deviceList.push_back(nDevice);

            Q_EMIT deviceAdded(nDevice);
        } else {
            deletedDevices.removeOne(id);
        }
    }

    for (QList<QString>::const_iterator it = deletedDevices.begin(), e = deletedDevices.end();
         it != e ; ++it) {
        QInputDevice * device = deviceMap.take(*it);
        deviceList.removeOne(device);
        Q_EMIT deviceRemoved(*it);
        delete device;
    }
    Q_EMIT ready();
}
