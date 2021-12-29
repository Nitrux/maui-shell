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

#ifndef QDEVICEKITSERVICE_H
#define QDEVICEKITSERVICE_H

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

#include <QtDBus/QtDBus>

QT_BEGIN_NAMESPACE


class QUPowerInterface : public QDBusAbstractInterface
{
    Q_OBJECT

public:
    QUPowerInterface(QObject *parent = 0);
    ~QUPowerInterface();

    QList<QDBusObjectPath> enumerateDevices();

Q_SIGNALS:
    void deviceAdded(const QString &path);
    void deviceRemoved(const QString &path);

protected:
    void connectNotify(const QMetaMethod &signal);
    void disconnectNotify(const QMetaMethod &signal);

private Q_SLOTS:
    void onDeviceAdded(const QDBusObjectPath &path);
    void onDeviceRemoved(const QDBusObjectPath &path);
};

class QUPowerDeviceInterface : public  QDBusAbstractInterface
{
    Q_OBJECT
public:
    QUPowerDeviceInterface(const QString &dbusPathName,QObject *parent = 0);
    ~QUPowerDeviceInterface();


    bool isPowerSupply();
    bool isOnline();
    double currentEnergy();
    double energyWhenFull();
    double energyDischargeRate();
    double percentLeft();
    double voltage();
    quint16 state();
    quint16 type();
    qint64 timeToFull();
    QString nativePath();
    quint16 technology();

    QVariantMap getProperties() { return pMap; }

Q_SIGNALS:
    void changed();
    void propertyChanged(QString,QVariant);

protected:
    void connectNotify(const QMetaMethod &signal);
    void disconnectNotify(const QMetaMethod &signal);

private:
    QVariantMap pMap;

private Q_SLOTS:
    void propChanged();
    void propRequestFinished(QDBusPendingCallWatcher *call);
};


QT_END_NAMESPACE

#endif // QDEVICEKITSERVICE_H
