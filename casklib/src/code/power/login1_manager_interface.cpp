/*
    SPDX-FileCopyrightText: 2019 David Edmundson <daviedmundson@kde.org>

    SPDX-License-Identifier: LGPL-2.0-only
*/

// This file overrides the auto-generated login1_manager_interface.cpp in order
// change the interface name and act as a completely transparent Logind1 / CK2 wrapper

#include "login1_manager_interface.h"

enum LogindProvider {
    Unknown,
    None,
    ConsoleKit2,
    Logind1,
};
static LogindProvider s_mode = Unknown;

static LogindProvider getLogindMode()
{
    if (s_mode != Unknown) {
        return s_mode;
    }
    if (QDBusConnection::systemBus().interface()->isServiceRegistered(QStringLiteral("org.freedesktop.login1"))) {
        s_mode = Logind1;
    } else if (QDBusConnection::systemBus().interface()->isServiceRegistered(QStringLiteral("org.freedesktop.ConsoleKit"))) {
        s_mode = ConsoleKit2;
    } else {
        s_mode = None;
    }
    return s_mode;
}

OrgFreedesktopLogin1ManagerInterface::OrgFreedesktopLogin1ManagerInterface(const QString &, const QString &, const QDBusConnection &connection, QObject *parent)
    : QDBusAbstractInterface(getLogindMode() == Logind1 ? QStringLiteral("org.freedesktop.login1") : QStringLiteral("org.freedesktop.ConsoleKit"),
                             getLogindMode() == Logind1 ? QStringLiteral("/org/freedesktop/login1") : QStringLiteral("/org/freedesktop/ConsoleKit/Manager"),
                             getLogindMode() == Logind1 ? "org.freedesktop.login1.Manager" : "org.freedesktop.ConsoleKit.Manager",
                             connection,
                             parent)
{
}

OrgFreedesktopLogin1ManagerInterface::~OrgFreedesktopLogin1ManagerInterface()
{
}

#include "login1_manager_interface.moc"
