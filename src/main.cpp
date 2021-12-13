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
#include <QDir>
#include <QElapsedTimer>
#include <QFontDatabase>
#include <QApplication>
#include <QScreen>
#include <QUrl>
#include <QWindow>

#include <QtQml/qqml.h>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickItem>
#include <QQuickStyle>

#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

//models and controllers by Cask
#include "code/controllers/zpaces.h"
#include "code/controllers/zpace.h"
#include "code/models/zpacesmodel.h"
#include "code/models/surfacesmodel.h"
#include "code/controllers/xdgwindow.h"
#include "code/controllers/waylandcursorgrabber.h"

static QByteArray grefsonExecutablePath;
static qint64 grefsonPID;
static void *signalHandlerStack;
static QString logFilePath;
static QElapsedTimer sinceStartup;

QString grefsenConfigDirPath(QDir::homePath() + "/.config/grefsen/");

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
        kill(grefsonPID, 9);
        fprintf(stderr, "crashed (PID %lld SIG %d): respawn %s\n", grefsonPID, signal, grefsonExecutablePath.constData());
        execl(grefsonExecutablePath.constData(), grefsonExecutablePath.constData(), (char *) 0);
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


static qreal highestDPR(QList<QScreen *> &screens)
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

#define ZPACES_URI "Zpaces"
int main(int argc, char *argv[])
{
    sinceStartup.start();

//    if (!qEnvironmentVariableIsSet("QT_XCB_GL_INTEGRATION"))
//        qputenv("QT_XCB_GL_INTEGRATION", "xcb_egl"); // use xcomposite-glx if no EGL
    if (!qEnvironmentVariableIsSet("QT_WAYLAND_DISABLE_WINDOWDECORATION"))
        qputenv("QT_WAYLAND_DISABLE_WINDOWDECORATION", "1");


//    if (!qEnvironmentVariableIsSet("QT_QUICK_CONTROLS_STYLE"))
//        qputenv("QT_QUICK_CONTROLS_STYLE", "maui-style");
    if (!qEnvironmentVariableIsSet("QT_QPA_PLATFORMTHEME"))
        qputenv("QT_QPA_PLATFORMTHEME", "generic");

//        if (!qEnvironmentVariableIsSet("QT_QUICK_CONTROLS_MOBILE"))
//    qputenv("QT_QUICK_CONTROLS_MOBILE", "1");




    // ShareOpenGLContexts is needed for using the threaded renderer
       // on NVIDIA EGLStreams and multi output compositors in general
       // (see QTBUG-63039 and QTBUG-87597)
       QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

       // Automatically support HiDPI
       QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);


    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));
//    qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "1");

    QGuiApplication app(argc, argv);
    QCoreApplication::setApplicationName("Cask"); // defaults to name of the executable
    QApplication::setOrganizationName("maui");
    QApplication::setApplicationVersion("1.0");
#ifndef QT_NO_SESSIONMANAGER
    app.setFallbackSessionManagementEnabled(false);
#endif
    app.setQuitOnLastWindowClosed(false);

    QQuickStyle::setStyle("maui-style");

    grefsonExecutablePath = app.applicationFilePath().toLocal8Bit();
    grefsonPID = QCoreApplication::applicationPid();
    bool windowed = false;

    QList<QScreen *> screens = QGuiApplication::screens();

        QCommandLineParser parser;
        parser.setApplicationDescription("Grefsen Qt/Wayland compositor");
        parser.addHelpOption();
        parser.addVersionOption();

        QCommandLineOption respawnOption(QStringList() << "r" << "respawn",
                                         QCoreApplication::translate("main", "respawn grefsen after a crash"));
        parser.addOption(respawnOption);

        QCommandLineOption logFileOption(QStringList() << "l" << "log",
                                         QCoreApplication::translate("main", "redirect all debug/warning/error output to a log file"),
                                         QCoreApplication::translate("main", "file path"));
        parser.addOption(logFileOption);

        QCommandLineOption configDirOption(QStringList() << "c" << "config",
                                           QCoreApplication::translate("main", "load config files from the given directory (default is ~/.config/grefsen)"),
                                           QCoreApplication::translate("main", "directory path"));
        parser.addOption(configDirOption);

        QCommandLineOption screenOption(QStringList() << "s" << "screen",
                                        QCoreApplication::translate("main", "send output to the given screen"),
                                        QCoreApplication::translate("main", "screen"));
        parser.addOption(screenOption);

        QCommandLineOption windowOption(QStringList() << "w" << "window",
                                        QCoreApplication::translate("main", "run in a window rather than fullscreen"));
        parser.addOption(windowOption);

        parser.process(app);
        if (parser.isSet(respawnOption))
            setupSignalHandler();
        if (parser.isSet(configDirOption))
            grefsenConfigDirPath = parser.value(configDirOption);
        if (parser.isSet(logFileOption)) {
            logFilePath = parser.value(logFileOption);
            qInstallMessageHandler(qtMsgLog);
        }
        if (parser.isSet(screenOption)) {
            QStringList scrNames = parser.values(screenOption);
            QList<QScreen *> keepers;
            foreach (QScreen *scr, screens)
                if (scrNames.contains(scr->name(), Qt::CaseInsensitive))
                    keepers << scr;
            if (keepers.isEmpty()) {
                qWarning() << "None of the screens" << scrNames << "exist; available screens:";
                foreach (QScreen *scr, screens)
                    qWarning() << "   " << scr->name() << scr->geometry();
                return -1;
            }
            screens = keepers;
        }
        if (parser.isSet(windowOption))
            windowed = true;

        qreal dpr = highestDPR(screens);
        if (!qEnvironmentVariableIsSet("XCURSOR_SIZE")) {
            // QTBUG-67579: we can't have different cursor sizes on different screens
            // or different windows yet, but we can override globally
            int cursorSize = int(32 * dpr);
            qDebug() << "highest DPR" << dpr << "-> cursor size" << cursorSize;
            qputenv("XCURSOR_SIZE", QByteArray::number(cursorSize));
        }


        qputenv("QT_QPA_PLATFORM", "wayland"); // not for grefsen but for child processes
        qputenv("GDK_BACKEND", "wayland"); // not for grefsen but for child processes
    qputenv("MOZ_ENABLE_WAYLAND", "1");

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);


    qmlRegisterUncreatableType<Zpace>(ZPACES_URI, 1, 0, "Zpace", "Use it from a reference form Zpaces object");
    qmlRegisterAnonymousType<ZpacesModel>(ZPACES_URI, 1);
    qmlRegisterAnonymousType<SurfacesModel>(ZPACES_URI, 1);
    qmlRegisterAnonymousType<AbstractWindow>(ZPACES_URI, 1);
    qmlRegisterType<Zpaces>(ZPACES_URI, 1, 0, "Zpaces");
    qmlRegisterType<XdgWindow>(ZPACES_URI, 1, 0, "XdgWindow");
    qmlRegisterType<WaylandCursorGrabber>(ZPACES_URI, 1, 0, "WaylandCursorGrabber");
    engine.load(url);


    QObject *root = engine.rootObjects().first();

    QList<QWindow *> windows = root->findChildren<QWindow *>();
    auto windowIter = windows.begin();
    auto screenIter = screens.begin();
    while (windowIter != windows.end() && screenIter != screens.end()) {
        QWindow * window = *windowIter;
        QScreen * screen = *screenIter;
        window->setScreen(screen);
        if (windowed) {
            window->showNormal();
        } else {
            window->setGeometry(screen->geometry());
            window->showFullScreen();
        }
        ++windowIter;
        ++screenIter;
    }

    return app.exec();
}
