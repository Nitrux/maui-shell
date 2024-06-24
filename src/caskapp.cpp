#include "caskapp.h"

#include <QCoreApplication>
#include <QDir>
#include <QDBusConnection>
#include <QDBusError>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlExtensionPlugin>
#include <QStandardPaths>
#include <QtPlugin>
#include <QQuickItem>
#include <QWindow>

#include <KLocalizedString>

#include <unistd.h>
#include <sys/types.h>

#include "code/controllers/zpaces.h"
#include "code/controllers/zpace.h"
#include "code/models/zpacesmodel.h"
#include "code/models/surfacesmodel.h"
#include "code/models/tasksmodel.h"
#include "code/controllers/task.h"
#include "code/controllers/xdgwindow.h"
#include "code/controllers/waylandcursorgrabber.h"

#define ZPACES_URI "Zpaces"

static const QEvent::Type StartupEventType = static_cast<QEvent::Type>(QEvent::registerEventType());

static int convertPermission(const QFileInfo &fileInfo)
{
    int p = 0;

    if (fileInfo.permission(QFile::ReadUser))
        p += 400;
    if (fileInfo.permission(QFile::WriteUser))
        p += 200;
    if (fileInfo.permission(QFile::ExeUser))
        p += 100;
    if (fileInfo.permission(QFile::ReadGroup))
        p += 40;
    if (fileInfo.permission(QFile::WriteGroup))
        p += 20;
    if (fileInfo.permission(QFile::ExeGroup))
        p += 10;
    if (fileInfo.permission(QFile::ReadOther))
        p += 4;
    if (fileInfo.permission(QFile::WriteOther))
        p += 2;
    if (fileInfo.permission(QFile::ExeOther))
        p += 1;

    return p;
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
    qmlRegisterType<WaylandCursorGrabber>(ZPACES_URI, 1, 0, "WaylandCursorGrabber");
}


CaskApp::CaskApp(QObject *parent) : QObject(parent)
    , m_failSafe(false)
    , m_started(false)
    , m_windowed(false)
{
    m_screens = QGuiApplication::screens();
    registerTypes();

           // Application engine
    m_appEngine = new QQmlApplicationEngine(this);
    connect(m_appEngine, &QQmlApplicationEngine::objectCreated, this, &CaskApp::objectCreated);

           // Invoke shutdown sequence when quitting
    connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, this, &CaskApp::shutdown);
}

void CaskApp::setUrl(const QUrl &url)
{
    m_url = url;
}

void CaskApp::setWindowed(bool value)
{
    m_windowed = value;
}

bool CaskApp::setScreens(const QStringList &scrNames)
{
    QList<QScreen *> keepers;
    for (QScreen *scr : m_screens)
    {
        if (scrNames.contains(scr->name(), Qt::CaseInsensitive))
        {
            keepers << scr;
        }
    }

    if (keepers.isEmpty())
    {
        qWarning() << "None of the screens" << scrNames << "exist; available screens:";
        for (QScreen *scr : m_screens)
        {
            qWarning() << "   " << scr->name() << scr->geometry();
        }
        return false;
    }
    m_screens = keepers;

    return true;
}

void CaskApp::customEvent(QEvent *event)
{
    if (event->type() == StartupEventType)
        startup();
}

void CaskApp::verifyXdgRuntimeDir()
{
    QString dirName = QString::fromLocal8Bit(qgetenv("XDG_RUNTIME_DIR"));

    if (dirName.isEmpty()) {
        QString msg = QObject::tr(
            "The XDG_RUNTIME_DIR environment variable is not set.\n"
            "Refer to your distribution on how to get it, or read\n"
            "http://www.freedesktop.org/wiki/Specifications/basedir-spec\n"
            "on how to implement it.\n");
        qFatal("%s", qPrintable(msg));
    }

    QFileInfo fileInfo(dirName);

    if (!fileInfo.exists()) {
        QString msg = QObject::tr(
                          "The XDG_RUNTIME_DIR environment variable is set to "
                          "\"%1\", which doesn't exist.\n").arg(dirName.constData());
        qFatal("%s", qPrintable(msg));
    }

    if (convertPermission(fileInfo) != 700 || fileInfo.ownerId() != getuid()) {
        QString msg = QObject::tr(
                          "XDG_RUNTIME_DIR is set to \"%1\" and is not configured correctly.\n"
                          "Unix access mode must be 0700, but is 0%2.\n"
                          "It must also be owned by the current user (UID %3), "
                          "but is owned by UID %4 (\"%5\").\n")
                          .arg(dirName.constData())
                          .arg(convertPermission(fileInfo))
                          .arg(getuid())
                          .arg(fileInfo.ownerId())
                          .arg(fileInfo.owner());
        qFatal("%s\n", qPrintable(msg));
    }
}

void CaskApp::startup()
{
  // Can't do the startup sequence twice
    if (m_started)
        return;

           // Check whether XDG_RUNTIME_DIR is ok or not
    verifyXdgRuntimeDir();

           //        // Session interface
           // m_appEngine->rootContext()->setContextProperty(QStringLiteral("SessionInterface"),
           //                                                m_sessionManager);

    m_appEngine->rootContext()->setContextObject(new KLocalizedContext(m_appEngine));


    qunsetenv("QT_SCALE_FACTOR");

    qputenv("QT_QPA_PLATFORM", "wayland"); // not for cask but for child processes
    qputenv("GDK_BACKEND", "wayland"); // not for cask but for child processes
    qputenv("MOZ_ENABLE_WAYLAND", "1");

           // Load the compositor
    m_appEngine->load(m_url);

    m_started = true;
}

void CaskApp::shutdown()
{
    m_appEngine->deleteLater();
    m_appEngine = nullptr;

           // m_sessionManager->deleteLater();
           // m_sessionManager = nullptr;
}

void CaskApp::objectCreated(QObject *object, const QUrl &)
{
  // All went fine
    if (object)
    {

        for(const auto &screen : m_screens)
        {
            qWarning() << screen->devicePixelRatio() << screen->logicalDotsPerInch()<< screen->physicalDotsPerInch() << screen->availableGeometry() << screen->availableSize() <<screen->availableVirtualSize();
        }

        QObject *root = m_appEngine->rootObjects().first();

        QList<QWindow *> windows = root->findChildren<QWindow *>();
        auto windowIter = windows.begin();
        auto screenIter = m_screens.begin();
        while (windowIter != windows.end() && screenIter != m_screens.end())
        {
            QWindow * window = *windowIter;
            QScreen * screen = *screenIter;
            window->setScreen(screen);

            if (m_windowed)
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
        return;
    }

           // Compositor failed to load
    if (m_failSafe) {
        // We give up because even the error screen has an error
        qWarning("A fatal error has occurred while running Liri Shell, but the error "
            "screen has errors too. Giving up.");
        QCoreApplication::exit(1);
    } else {
        // Load the error screen in case of error
        m_failSafe = true;
        m_appEngine->load(QUrl(QStringLiteral("qrc:/qt/qml/compositor/qml/error/ErrorCompositor.qml")));
    }
}

StartupEvent::StartupEvent()
    : QEvent(StartupEventType)
{
}
