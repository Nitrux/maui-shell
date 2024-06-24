/****************************************************************************
**
** Copyright (C) 2015 Shawn Rutledge
**
** This file is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 3 as published by the Free Software Foundation
** and appearing in the file LICENSE included in the packaging
** of this file.
**
** This code is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU Lesser General Public License for more details.
**
****************************************************************************/

#include <QCommandLineParser>
#include <QDebug>
#include <QDate>
#include <QDir>
#include <QElapsedTimer>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QScreen>
#include <QUrl>
#include <QWindow>

#include <QtQml/qqml.h>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickItem>
#include <QQuickStyle>
#include <QSessionManager>
#include <QIcon>

#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <MauiKit4/Core/mauiapp.h>
#include <MauiKit4/Calendar/moduleinfo.h>

#include <KLocalizedString>

#include <CaskServer/caskpower.h>

#include "../cask_version.h"
#include "caskapp.h"

#include <MauiMan4/screenmanager.h>
#include <MauiMan4/thememanager.h>
#include <MauiMan4/mauimanutils.h>

#define ZPACES_URI "Zpaces"
#define CASK_URI "org.maui.cask"

static QByteArray caskExecutablePath;
static qint64 caskPID;
static void *signalHandlerStack;
static QString logFilePath;
static QElapsedTimer sinceStartup;

extern "C" void signalHandler(int signal)
{
#ifdef Q_WS_X11
  // Kill window since it's frozen anyway.
    if (QX11Info::display())
        close(ConnectionNumber(QX11Info::display()));
#endif
    pid_t pid = fork();
    switch (pid) {
    case -1: // error
        break;
    case 0: // child
        kill(caskPID, 9);
        fprintf(stderr, "crashed (PID %lld SIG %d): respawn %s\n", caskPID, signal, caskExecutablePath.constData());
        execl(caskExecutablePath.constData(), caskExecutablePath.constData(), (char *) 0);
        _exit(EXIT_FAILURE);
    default: // parent
        prctl(PR_SET_PTRACER, pid, 0, 0, 0);
        waitpid(pid, 0, 0);
        _exit(EXIT_FAILURE);
        break;
    }
}

static void setupSignalHandler()
{
    // Setup an alternative stack for the signal handler. This way we are able to handle SIGSEGV
    // even if the normal process stack is exhausted.
    stack_t ss;
    ss.ss_sp = signalHandlerStack = malloc(SIGSTKSZ); // Usual requirements for alternative signal stack.
    if (ss.ss_sp == 0) {
        qWarning("Warning: Could not allocate space for alternative signal stack (%s).", Q_FUNC_INFO);
        return;
    }
    ss.ss_size = SIGSTKSZ;
    ss.ss_flags = 0;
    if (sigaltstack(&ss, 0) == -1) {
        qWarning("Warning: Failed to set alternative signal stack (%s).", Q_FUNC_INFO);
        return;
    }

           // Install signal handler.
    struct sigaction sa;
    if (sigemptyset(&sa.sa_mask) == -1) {
        qWarning("Warning: Failed to empty signal set (%s).", Q_FUNC_INFO);
        return;
    }
    sa.sa_handler = &signalHandler;
    // SA_RESETHAND - Restore signal action to default after signal handler has been called.
    // SA_NODEFER - Don't block the signal after it was triggered (otherwise blocked signals get
    // inherited via fork() and execve()). Without this the signal will not be delivered to the
    // restarted Qt Creator.
    // SA_ONSTACK - Use alternative stack.
    sa.sa_flags = SA_RESETHAND | SA_NODEFER | SA_ONSTACK;
    // See "man 7 signal" for an overview of signals.
    // Do not add SIGPIPE here, QProcess and QTcpSocket use it.
    const int signalsToHandle[] = { SIGILL, SIGABRT, SIGFPE, SIGSEGV, SIGBUS, 0 };
    for (int i = 0; signalsToHandle[i]; ++i)
        if (sigaction(signalsToHandle[i], &sa, 0) == -1)
            qWarning("Failed to install signal handler for signal \"%s\"", strsignal(signalsToHandle[i]));
}

void qtMsgLog(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QFile log(logFilePath);
    if (!log.isOpen())
        log.open(QIODevice::WriteOnly | QIODevice::Text);
    char typeChar = ' ';
    switch (type) {
    case QtDebugMsg:
        typeChar = 'd';
        break;
    case QtInfoMsg:
        typeChar = 'i';
        break;
    case QtWarningMsg:
        typeChar = 'W';
        break;
    case QtCriticalMsg:
        typeChar = '!';
        break;
    case QtFatalMsg:
        typeChar = 'F';
    }
    char buf[64];
    qint64 ts = sinceStartup.elapsed();
    snprintf(buf, 64, "[%6lld.%03lld %c] ", ts / 1000, ts % 1000, typeChar);
    log.write(buf);
    if (context.function) {
        snprintf(buf, 64, "%s:%d: ", context.function, context.line);
        log.write(buf);
    }
    log.write(msg.toUtf8());
    log.write("\n");
    if (type == QtFatalMsg)
        abort();
}

static qreal highestDPR(QList<QScreen *> screens)
{
    qreal ret = 0;
    for (const QScreen *scr : screens) {
        qDebug() << "Screen" << scr->name() << scr->geometry() << scr->physicalSize()
                 << "DPI: log" << scr->logicalDotsPerInch() << "phys" << scr->physicalDotsPerInch()
                 << "DPR" << scr->devicePixelRatio();
        ret = qMax(ret, scr->devicePixelRatio());
    }
    return ret;
}

void sigintHandler(int signalNumber)
{
    qDebug() << "terminating cask session" << signalNumber;
}

static void setupEnviroment()
{
    if (!qEnvironmentVariableIsSet("QT_XCB_GL_INTEGRATION"))
        qputenv("QT_XCB_GL_INTEGRATION", "xcb_egl"); // use xcomposite-glx if no EGL

           //    if (!qEnvironmentVariableIsSet("QT_WAYLAND_DISABLE_WINDOWDECORATION"))
           //        qputenv("QT_WAYLAND_DISABLE_WINDOWDECORATION", "1");

    if (!qEnvironmentVariableIsSet("QT_LABS_CONTROLS_STYLE"))
        qputenv("QT_LABS_CONTROLS_STYLE", "Universal");

           //    if (!qEnvironmentVariableIsSet("QT_QUICK_CONTROLS_STYLE"))
           //        qputenv("QT_QUICK_CONTROLS_STYLE", "maui-style");

    if (!qEnvironmentVariableIsSet("QT_QPA_PLATFORMTHEME"))
        qputenv("QT_QPA_PLATFORMTHEME", "maui");

           //        if (!qEnvironmentVariableIsSet("QT_QUICK_CONTROLS_MOBILE"))
           //    qputenv("QT_QUICK_CONTROLS_MOBILE", "1");
    qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "0");

           //    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));
    {
        MauiMan::ScreenManager screenManager;
        qputenv("QT_SCALE_FACTOR", QByteArray::number(screenManager.scaleFactor()));
    }


}

int main(int argc, char *argv[])
{
    sinceStartup.start();

    setupEnviroment();
    // ShareOpenGLContexts is needed for using the threaded renderer
    // on NVIDIA EGLStreams and multi output compositors in general
    // (see QTBUG-63039 and QTBUG-87597)
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    QGuiApplication app(argc, argv);
    app.setOrganizationName(QStringLiteral("Maui"));
    app.setWindowIcon(QIcon(QStringLiteral("qrc:/mauikit-logo.png")));

    MauiApp::instance()->setIconName(QStringLiteral("qrc:/mauikit-logo.png"));

    KLocalizedString::setApplicationDomain("cask");
    KAboutData about(QStringLiteral("cask"),
                     QStringLiteral("Cask"),
                     QStringLiteral(CASK_VERSION_STRING),
                     i18n("Convergent shell for desktop and mobile computers."),
                     KAboutLicense::LGPL_V3,
                     QStringLiteral("© 2021-2024 Maui Development Team"),
                     QString(QStringLiteral(GIT_BRANCH) + QStringLiteral("/") + QStringLiteral(GIT_COMMIT_HASH)));

    about.addAuthor(QStringLiteral("Camilo Higuita"), i18n("Developer"), QStringLiteral("milo.h@aol.com"));
    about.setHomepage(QStringLiteral("https://mauikit.org"));
    about.setProductName("maui/cask");
    about.setBugAddress("https://github.com/Nitrux/maui-shell/issues/new/choose");
    about.setOrganizationDomain(CASK_URI);
    about.setProgramLogo(app.windowIcon());

    const auto FBData = MauiKitCalendar::aboutData();
    about.addComponent(FBData.name(), MauiKitCalendar::buildVersion(), FBData.version(), FBData.webAddress());

    KAboutData::setApplicationData(about);

    if(MauiManUtils::isMauiSession())
    {
        MauiMan::ThemeManager themeSettings;
        QIcon::setThemeName(themeSettings.iconTheme());
    }

    signal(SIGINT, sigintHandler);
    signal(SIGTSTP, sigintHandler);

    app.setQuitOnLastWindowClosed(false);


           //    auto power = new CaskPower();
           //    QObject::connect(power, &CaskPower::logoutRequested, [&app, power]()
           //    {
           //        power->deleteLater();
           //        app.quit();
           //    });


    QQuickStyle::setStyle(QStringLiteral("QtQuick.Controls.Maui"));

    caskExecutablePath = app.applicationFilePath().toLocal8Bit();
    caskPID = QCoreApplication::applicationPid();


    QCommandLineParser parser;
    about.setupCommandLine(&parser);

    parser.setApplicationDescription(about.shortDescription());
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption respawnOption(QStringList() << QStringLiteral("r") << QStringLiteral("respawn"),
                                     QCoreApplication::translate("main", "respawn cask after a crash"));
    parser.addOption(respawnOption);

    QCommandLineOption logFileOption(QStringList() << QStringLiteral("l") << QStringLiteral("log"),
                                     QCoreApplication::translate("main", "redirect all debug/warning/error output to a log file"),
                                     QCoreApplication::translate("main", "file path"));
    parser.addOption(logFileOption);

    QCommandLineOption screenOption(QStringList() << QStringLiteral("s") << QStringLiteral("screen"),
                                    QCoreApplication::translate("main", "send output to the given screen"),
                                    QCoreApplication::translate("main", "screen"));
    parser.addOption(screenOption);

    QCommandLineOption windowOption(QStringList() << QStringLiteral("w") << QStringLiteral("window"),
                                    QCoreApplication::translate("main", "run in a window rather than fullscreen"));
    parser.addOption(windowOption);

    parser.process(app);
    about.processCommandLine(&parser);

    if (parser.isSet(respawnOption))
        setupSignalHandler();

    if (parser.isSet(logFileOption))
    {
        logFilePath = parser.value(logFileOption);
        qInstallMessageHandler(qtMsgLog);
    }

    if (!qEnvironmentVariableIsSet("XCURSOR_SIZE"))
    {
        qreal dpr = highestDPR(QGuiApplication::screens());

        // QTBUG-67579: we can't have different cursor sizes on different screens
        // or different windows yet, but we can override globally
        int cursorSize = int(32 * dpr);
        qDebug() << "highest DPR" << dpr << "-> cursor size" << cursorSize;
        qputenv("XCURSOR_SIZE", QByteArray::number(cursorSize));
    }

    auto shell = new CaskApp();

    if (parser.isSet(screenOption))
    {
        shell->setScreens(parser.values(screenOption));
    }

    if (parser.isSet(windowOption))
    {
        shell->setWindowed("true");
    }

    const QUrl url(u"qrc:qt/qml/org/cask/shell/qml/main.qml"_qs);
    shell->setUrl(url);

    QCoreApplication::postEvent(shell, new StartupEvent());

    return app.exec();
}
