#include "caskpolkitplugin.h"

#include <QQmlEngine>
#include "dialog.h"
#include "agent.h"
#include "polkitagentlistener.h"

void CaskPolkitPlugin::registerTypes(const char *uri)
{    
    qmlRegisterType<Agent>(uri, 1, 0, "Agent");
    qmlRegisterUncreatableType<Dialog>(uri, 1, 0, "Dialog", "To be reference from the polkit agent controller.");
    qmlRegisterUncreatableType<PolKitAgentListener>(uri, 1, 0, "Listener", "To be reference from the polkit agent controller.");

    qmlRegisterType(resolveFileUrl(QStringLiteral("PolkitDialog.qml")), uri, 1, 0, "PolkitDialog");

}
