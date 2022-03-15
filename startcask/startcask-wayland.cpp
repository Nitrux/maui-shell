/*
    SPDX-FileCopyrightText: 2019 Aleix Pol Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "debug.h"
#include "startcask.h"
#include <KConfig>
#include <KConfigGroup>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusArgument>

#include <signal.h>
#include <QCoreApplication>

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    qputenv("QT_QPA_PLATFORM", "eglfs");
    qputenv("QT_QUICK_CONTROLS_STYLE", "maui-style");
    qputenv("QT_WAYLAND_CLIENT_BUFFER_INTEGRATION", "wayland-egl");

   qputenv("QT_QPA_ENABLE_TERMINAL_KEYBOARD", "1");

    createConfigDirectory();
    setupCursor();
//    signal(SIGINT, sigHandler);
    signal(SIGTERM, sigtermHandler);

    {
        KConfig fonts(QStringLiteral("kcmfonts"));
        KConfigGroup group = fonts.group("General");
        auto dpiSetting = group.readEntry("forceFontDPIWayland", 96);
        auto dpi = dpiSetting == 0 ? 96 : dpiSetting;
        qputenv("QT_WAYLAND_FORCE_DPI", QByteArray::number(dpi));
    }

    // Query whether org.freedesktop.locale1 is available. If it is, try to
    // set XKB_DEFAULT_{MODEL,LAYOUT,VARIANT,OPTIONS} accordingly.
    {
        const QString locale1Service = QStringLiteral("org.freedesktop.locale1");
        const QString locale1Path = QStringLiteral("/org/freedesktop/locale1");
        QDBusMessage message =
            QDBusMessage::createMethodCall(locale1Service, locale1Path, QStringLiteral("org.freedesktop.DBus.Properties"), QLatin1String("GetAll"));
        message << locale1Service;
        QDBusMessage resultMessage = QDBusConnection::systemBus().call(message);
        if (resultMessage.type() == QDBusMessage::ReplyMessage) {
            QVariantMap result;
            QDBusArgument dbusArgument = resultMessage.arguments().at(0).value<QDBusArgument>();
            while (!dbusArgument.atEnd()) {
                dbusArgument >> result;
            }

            auto queryAndSet = [&](const QByteArray &var, const QString &value) {
                const auto r = result.value(value).toString();
                if (!r.isEmpty())
                    qputenv(var, r.toUtf8());
            };

            queryAndSet("XKB_DEFAULT_MODEL", QStringLiteral("X11Model"));
            queryAndSet("XKB_DEFAULT_LAYOUT", QStringLiteral("X11Layout"));
            queryAndSet("XKB_DEFAULT_VARIANT", QStringLiteral("X11Variant"));
            queryAndSet("XKB_DEFAULT_OPTIONS", QStringLiteral("X11Options"));
        } else {
            qCWarning(CASK_STARTUP) << "not a reply org.freedesktop.locale1" << resultMessage;
        }
    }

    runEnvironmentScripts();

    if (!qEnvironmentVariableIsSet("DBUS_SESSION_BUS_ADDRESS")) {
        out << "startcask session: Could not start D-Bus. Can you call qdbus?\n";
        return 1;
    }

    setupCaskEnvironment();
    runStartupConfig();

    qputenv("PLASMA_USE_QT_SCALING", "1");
    qputenv("GDK_SCALE", "1");
    qputenv("GDK_DPI_SCALE", "1");

    qputenv("XDG_SESSION_TYPE", "wayland");

    auto oldSystemdEnvironment = getSystemdEnvironment();
    if (!syncDBusEnvironment()) {
        out << "Could not sync environment to dbus.\n";
        return 1;
    }

    // We import systemd environment after we sync the dbus environment here.
    // Otherwise it may leads to some unwanted order of applying environment
    // variables (e.g. LANG and LC_*)
    importSystemdEnvrionment();

    if (!startCaskSession())
        return 4;

    // Anything after here is logout
    // It is not called after shutdown/restart
    waitForKonqi();
    out << "startcask-wayland: Shutting down...\n";

    // Keep for KF5; remove in KF6 (KInit will be gone then)
    runSync(QStringLiteral("kdeinit5_shutdown"), {});

    out << "startplasmacompositor: Shutting down...\n";
    cleanupPlasmaEnvironment(oldSystemdEnvironment);
    out << "startplasmacompositor: Done.\n";

    return 0;
}
