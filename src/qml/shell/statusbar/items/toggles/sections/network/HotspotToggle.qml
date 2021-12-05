import QtQuick 2.15
import QtQuick.Controls 2.5

import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.2 as Maui
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM

import org.maui.cask 1.0 as Cask
import "../.."

ToggleTemplate
{
    id: control

    visible: handler.hotspotSupported
    checkable: true

    text: i18n("Hotspot")
    icon.name: "network-wireless-on"

    onClicked: {
        if (PlasmaNM.Configuration.hotspotConnectionPath) {
            checked = false
            handler.stopHotspot()
        } else {
            checked = true
            handler.createHotspot()
        }
    }

    Connections {
        target: handler
        function onHotspotCreated() {
            control.checked = true
            tooltip.text = i18n("Disable Hotspot")
        }

        function onHotspotDisabled() {
            control.checked = false
            tooltip.text = i18n("Create Hotspot")
        }
    }

    Component.onCompleted: {
        checked = PlasmaNM.Configuration.hotspotConnectionPath
        tooltip.text = PlasmaNM.Configuration.hotspotConnectionPath ? i18n("Disable Hotspot") : i18n("Create Hotspot")
    }



}
