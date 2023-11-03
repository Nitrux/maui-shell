import QtQuick
import QtQuick.Controls

import org.mauikit.controls as Maui
import org.kde.plasma.networkmanagement as PlasmaNM

import org.maui.cask as Cask
import "../.."

ToggleTemplate
{
    id: control
    checked: PlasmaNM.Configuration.airplaneModeEnabled
    checkable: true
    icon.name: "network-flightmode-on"
    text: i18n("Airplane Mode")

    visible: availableDevices.modemDeviceAvailable || availableDevices.wirelessDeviceAvailable

    onToggled:
    {
        handler.enableAirplaneMode(checked);
        PlasmaNM.Configuration.airplaneModeEnabled = checked;
    }

    PlasmaNM.AvailableDevices {
        id: availableDevices
    }

    PlasmaNM.Handler
    {
        id: handler
    }

}
