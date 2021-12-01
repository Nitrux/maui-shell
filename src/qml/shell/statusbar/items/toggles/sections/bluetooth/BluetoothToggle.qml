import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5

import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.2 as Maui

import org.maui.cask 1.0 as Cask
import "../.."

ToggleTemplate
{
    icon.name: "network-bluetooth"
    template.label2.text: "PinePhone-Pro-2x"
    template.label1.text: i18n("Bluetooth")
    page: TogglePage
    {

    }
}
