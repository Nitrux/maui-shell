#include "caskpowerplugin.h"


#include <QQmlEngine>

#include "brightnesscontrol.h"

void CaskAudioPlugin::registerTypes(const char *uri)
{
    qmlRegisterType<BrightnessControl>(uri, 1, 0, "BrightnessControl");
}
