#include "caskplugin.h"

void CaskPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("org.maui.cask"));
    qmlRegisterType(fromBase(QStringLiteral("Cask.qml")), uri, 1, 0, "Cask");
//    qmlRegisterType(fromBase(QStringLiteral("panels/statusbar/StatusBar.qml")), uri, 1, 0, "StatusBar");
//    qmlRegisterType(fromBase(QStringLiteral("panels/tasksbar/TaskBar.qml")), uri, 1, 0, "TaskBar");
//    qmlRegisterType(fromBase(QStringLiteral("templates/Panel.qml")), uri, 1, 0, "Panel");
//    qmlRegisterType(fromBase(QStringLiteral("templates/PanelCard.qml")), uri, 1, 0, "PanelCard");
//    qmlRegisterType(fromBase(QStringLiteral("templates/PanelItem.qml")), uri, 1, 0, "PanelItem");
//    qmlRegisterType(fromBase(QStringLiteral("templates/PanelPopup.qml")), uri, 1, 0, "PanelSection");
//    qmlRegisterType(fromBase(QStringLiteral("templates/PanelSection.qml")), uri, 1, 0, "PanelSection");

    qmlRegisterType<AppsModel>(uri, 1, 0, "AppsModel");
}
