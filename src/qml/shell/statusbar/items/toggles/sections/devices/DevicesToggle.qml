import QtQuick 2.15
import QtQuick.Controls 2.15

import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.3 as Maui

import org.maui.cask 1.0 as Cask
import "../.."

ToggleTemplate
{
    icon.name: "device-notifier"
    text:  "Devices"
    display: ToolButton.TextBesideIcon

    page: DevicesPage
    {

    }
}
