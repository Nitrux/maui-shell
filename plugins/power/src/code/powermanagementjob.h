/*
    SPDX-FileCopyrightText: 2011 Sebastian Kügler <sebas@kde.org>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#pragma once

// plasma
#include <QObject>

class QDBusPendingCall;

class PowerManagementJob : public QObject
{
    Q_OBJECT

public:
    PowerManagementJob(QObject *parent = nullptr);
    void start();

private:
    QDBusPendingCall setScreenBrightness(int value, bool silent);
    QDBusPendingCall setKeyboardBrightness(int value, bool silent);
    QDBusPendingCall setPowerProfile(const QString &value);
};
