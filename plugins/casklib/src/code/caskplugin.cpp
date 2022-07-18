#include "caskplugin.h"
#include "stackableitem.h"
#include "enviroment.h"
#include "models/appsmodel.h"
#include "models/recentfilesmodel.h"

#include "processlauncher.h"

#include "code/mpris2/mpris2engine.h"
#include "code/mpris2/mpris2player.h"
#include "code/mpris2/playersmodel.h"

#include "code/controllers/appsdb.h"

#include "code/controllers/mauimaninterface.h"

#include "code/server/caskserver.h"

void CaskPlugin::registerTypes(const char *uri)
{
    qmlRegisterType(resolveFileUrl(QStringLiteral("templates/PanelItem.qml")), uri, 1, 0, "PanelItem");

    qmlRegisterType(resolveFileUrl(QStringLiteral("templates/Panel.qml")), uri, 1, 0, "Panel");

    qmlRegisterType(resolveFileUrl(QStringLiteral("templates/PanelPopup.qml")), uri, 1, 0, "PanelPopup");

    qmlRegisterType(resolveFileUrl(QStringLiteral("templates/PanelCard.qml")), uri, 1, 0, "PanelCard");

    qmlRegisterType(resolveFileUrl(QStringLiteral("templates/PanelSection.qml")), uri, 1, 0, "PanelSection");

    qmlRegisterType(resolveFileUrl(QStringLiteral("templates/IconLabel.qml")), uri, 1, 0, "IconLabel");

    qmlRegisterType(resolveFileUrl(QStringLiteral("Dashboard.qml")), uri, 1, 0, "Dashboard");

    //controllers
    qmlRegisterType<StackableItem>(uri, 1, 0, "StackableItem");
    qmlRegisterSingletonType<Enviroment>(uri, 1, 0, "Env", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)
        return new Enviroment;
    });
    qmlRegisterType<WaylandProcessLauncher>(uri, 1, 0, "ProcessLauncher");

    qmlRegisterType<AppsModel>(uri, 1, 0, "AppsModel");
    qmlRegisterType<RecentFilesModel>(uri, 1, 0, "RecentFiles");
    qmlRegisterType<AppsDB>(uri, 1, 0, "AppsDB");

    //mpris stuff
    qmlRegisterAnonymousType<PlayersModel>(uri, 1);
    qmlRegisterType<Mpris2Engine>(uri, 1, 0, "Mpris");
    qmlRegisterUncreatableType<Mpris2Player>(uri, 1, 0, "MprisPlayer",
                                             QStringLiteral("Cannot create MprisPlayer object"));

    qmlRegisterSingletonType<MauiManInterface>(uri, 1, 0, "MauiMan", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)
        return new MauiManInterface;
    });

    qmlRegisterSingletonType<CaskServer>(uri,1,0, "Server", [](QQmlEngine *, QJSEngine*)
    {
        return CaskServer::instance();
    });
}

void CaskPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_UNUSED(engine)
    Q_UNUSED(uri)
}
