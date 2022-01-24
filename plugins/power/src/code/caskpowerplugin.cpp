#include "caskpowerplugin.h"


#include <QQmlEngine>

#include "brightnesscontrol.h"
#include "batteryinfo.h"

void CaskAudioPlugin::registerTypes(const char *uri)
{
    qmlRegisterType<BrightnessControl>(uri, 1, 0, "BrightnessControl");
    qmlRegisterType<BatteryInfo>(uri, 1, 0, "BatteryInfo");
    qmlRegisterUncreatableType<Battery>(uri, 1, 0, "Battery", "To be reference from the model or controller.");
}
