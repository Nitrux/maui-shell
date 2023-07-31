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
#include <KLocalizedString>


//models and controllers by Cask
#include "code/controllers/zpaces.h"
#include "code/controllers/zpace.h"
#include "code/models/zpacesmodel.h"
#include "code/models/surfacesmodel.h"
#include "code/models/tasksmodel.h"
#include "code/controllers/task.h"
#include "code/controllers/xdgwindow.h"
#include "code/controllers/waylandcursorgrabber.h"

#include <CaskServer/caskpower.h>

#include "../cask_version.h"

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

void sigintHandler(int signalNumber)
{
    qDebug() << "terminating cask session" << signalNumber;
}


static void registerTypes()
{
    qmlRegisterUncreatableType<Zpace>(ZPACES_URI, 1, 0, "Zpace", QStringLiteral("Use it from a reference from Zpaces object"));
    qmlRegisterUncreatableType<Task>(ZPACES_URI, 1, 0, "Task", QStringLiteral("Use it from Zpaces::TasksModel.task object"));
    qmlRegisterAnonymousType<ZpacesModel>(ZPACES_URI, 1);
    qmlRegisterAnonymousType<TasksModel>(ZPACES_URI, 1);
    qmlRegisterAnonymousType<SurfacesModel>(ZPACES_URI, 1);
    qmlRegisterAnonymousType<AbstractWindow>(ZPACES_URI, 1);
    qmlRegisterType<Zpaces>(ZPACES_URI, 1, 0, "Zpaces");
    qmlRegisterUncreatableType<XdgWindow>(ZPACES_URI, 1, 0, "XdgWindow", QStringLiteral("Create it from Zpaces::createXdgWIndow"));
    //    qmlRegisterType<WaylandCursorGrabber>(ZPACES_URI, 1, 0, "WaylandCursorGrabber");
}

int main(int argc, char *argv[])
{
    sinceStartup.start();

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


    // ShareOpenGLContexts is needed for using the threaded renderer
    // on NVIDIA EGLStreams and multi output compositors in general
    // (see QTBUG-63039 and QTBUG-87597)
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);


    qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "0");

    //    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));
    {
        MauiMan::ScreenManager screenManager;
        qputenv("QT_SCALE_FACTOR", QByteArray::number(screenManager.scaleFactor()));
    }

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
                     i18n("© 2021-%1 Maui Development Team", QString::number(QDate::currentDate().year())),
                     QString(QStringLiteral(GIT_BRANCH) + QStringLiteral("/") + QStringLiteral(GIT_COMMIT_HASH)));

    about.addAuthor(i18n("Camilo Higuita"), i18n("Developer"), QStringLiteral("milo.h@aol.com"));
    about.setHomepage(QStringLiteral("https://mauikit.org"));
    about.setProductName("maui/cask");
    about.setBugAddress("https://invent.kde.org/maui/vvave/-/issues");
    about.setOrganizationDomain(CASK_URI);
    about.setProgramLogo(app.windowIcon());

    KAboutData::setApplicationData(about);

    // if(MauiManUtils::isMauiSession())
    //    {
    //auto themeSettings =  std::make_unique<MauiMan::ThemeManager>();

    //        QIcon::setThemeName(themeSettings->iconTheme());
    //    }

    signal(SIGINT, sigintHandler);
    signal(SIGTSTP, sigintHandler);

    app.setQuitOnLastWindowClosed(false);
    //    QGuiApplication::setFallbackSessionManagementEnabled(false);
    auto disableSessionManagement = [](QSessionManager &sm) {
        sm.setRestartHint(QSessionManager::RestartNever);
    };

    //    auto power = new CaskPower();
    //    QObject::connect(power, &CaskPower::logoutRequested, [&app, power]()
    //    {
    //        power->deleteLater();
    //        app.quit();
    //    });

    QObject::connect(&app, &QGuiApplication::commitDataRequest, disableSessionManagement);
    QObject::connect(&app, &QGuiApplication::saveStateRequest, disableSessionManagement);

    QQuickStyle::setStyle(QStringLiteral("QtQuick.Controls.Maui"));

    caskExecutablePath = app.applicationFilePath().toLocal8Bit();
    caskPID = QCoreApplication::applicationPid();
    bool windowed = false;

    QList<QScreen *> screens = QGuiApplication::screens();

    for(const auto &screen : screens)
    {
        qWarning() << screen->devicePixelRatio() << screen->logicalDotsPerInch()<< screen->physicalDotsPerInch() << screen->availableGeometry() << screen->availableSize() <<screen->availableVirtualSize();
    }

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

    if (parser.isSet(screenOption))
    {
        QStringList scrNames = parser.values(screenOption);
        QList<QScreen *> keepers;
        for (QScreen *scr : screens)
        {
            if (scrNames.contains(scr->name(), Qt::CaseInsensitive))
            {
                keepers << scr;
            }
        }

        if (keepers.isEmpty())
        {
            qWarning() << "None of the screens" << scrNames << "exist; available screens:";
            for (QScreen *scr : screens)
            {
                qWarning() << "   " << scr->name() << scr->geometry();
            }
            return -1;
        }
        screens = keepers;
    }

    if (parser.isSet(windowOption))
        windowed = true;

    qreal dpr = highestDPR(screens);

    if (!qEnvironmentVariableIsSet("XCURSOR_SIZE"))
    {
        // QTBUG-67579: we can't have different cursor sizes on different screens
        // or different windows yet, but we can override globally
        int cursorSize = int(32 * dpr);
        qDebug() << "highest DPR" << dpr << "-> cursor size" << cursorSize;
        qputenv("XCURSOR_SIZE", QByteArray::number(cursorSize));
    }

    registerTypes();

    QQmlApplicationEngine engine;
    const QUrl url(u"qrc:/cask/qml/main.qml"_qs);

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));


    engine.load(url);

    qunsetenv("QT_SCALE_FACTOR");
    qputenv("QT_QPA_PLATFORM", "wayland"); // not for cask but for child processes
    qputenv("GDK_BACKEND", "wayland"); // not for cask but for child processes
    qputenv("MOZ_ENABLE_WAYLAND", "1");

    QObject *root = engine.rootObjects().first();

    QList<QWindow *> windows = root->findChildren<QWindow *>();
    auto windowIter = windows.begin();
    auto screenIter = screens.begin();
    while (windowIter != windows.end() && screenIter != screens.end())
    {
        QWindow * window = *windowIter;
        QScreen * screen = *screenIter;
        window->setScreen(screen);

        if (windowed)
        {
            window->showNormal();
        } else
        {
            window->setGeometry(screen->geometry());
            window->showFullScreen();
        }
        ++windowIter;
        ++screenIter;
    }

    return app.exec();
}
