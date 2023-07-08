#include "caskpolkitplugin.h"

#include <QQmlEngine>
#include "dialog.h"
#include "agent.h"
#include "polkitagentlistener.h"

void CaskPolkitPlugin::registerTypes(const char *uri)
{    
    qmlRegisterType<Agent>(uri, 1, 0, "Agent");
    qmlRegisterUncreatableType<Dialog>(uri, 1, 0, "Dialog", QStringLiteral("To be reference from the polkit agent controller."));
    qmlRegisterUncreatableType<PolKitAgentListener>(uri, 1, 0, "Listener", QStringLiteral("To be reference from the polkit agent controller."));

    qmlRegisterType(componentUrl(QStringLiteral("PolkitDialog.qml")), uri, 1, 0, "PolkitDialog");

}

QUrl CaskPolkitPlugin::componentUrl(const QString &fileName) const
{
    return QUrl(resolveFileUrl(fileName));
}
