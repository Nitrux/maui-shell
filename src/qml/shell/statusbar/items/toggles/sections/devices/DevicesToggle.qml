import QtQuick
import QtQuick.Controls

import org.mauikit.controls as Maui

import org.maui.cask as Cask
import "../.."

ToggleTemplate
{
    icon.name: "drive-removable-media-usb"
    text:  "Devices"
//    display: ToolButton.TextUnderIcon

    page: DevicesPage
    {

    }
}
