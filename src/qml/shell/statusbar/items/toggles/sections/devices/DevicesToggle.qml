import QtQuick 2.15
import QtQuick.Controls 2.15

import org.mauikit.controls 1.3 as Maui

import org.maui.cask 1.0 as Cask
import "../.."

ToggleTemplate
{
    icon.name: "drive-removable-media-usb"
    text:  "Devices"
    display: ToolButton.TextUnderIcon

    page: DevicesPage
    {

    }
}
