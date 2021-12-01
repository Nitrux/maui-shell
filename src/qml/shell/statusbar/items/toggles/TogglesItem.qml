import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5

import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.2 as Maui

import org.maui.cask 1.0 as Cask
import "sections/network"
import "sections/bluetooth"

Cask.PanelItem
{
    id: control
    iconSize: 16

    Row
    {
        spacing: Maui.Style.space.medium
        Kirigami.Icon
        {
            source: "settings-configure"
            height: control.iconSize
            width: height
            color: control.hovered || control.down || control.pressed ? Kirigami.Theme.highlightColor : Kirigami.Theme.textColor
        }
        Kirigami.Icon
        {
            source: "network-bluetooth"
            height: control.iconSize
            width: height
            color: control.hovered || control.down || control.pressed ? Kirigami.Theme.highlightColor : Kirigami.Theme.textColor
        }

        Kirigami.Icon
        {
            source: "network-wireless"
            height: control.iconSize
            width: height
            color: control.hovered || control.down || control.pressed ? Kirigami.Theme.highlightColor : Kirigami.Theme.textColor
        }
    }

    card: Cask.PanelCard
    {
        width: parent.width

        StackView
        {
            id : _togglesStack
            width: parent.width
            height: currentItem.implicitHeight
            clip: true
            initialItem: Flow
            {
                id:_tooglesGrid

                spacing: Maui.Style.space.medium


                //                        model: ["network-bluetooth", "input-keyboard-virtual", "rotation-allowed","webcam", "accessories-calculator",  "settings-configure"]
                BluetoothToggle
                {
                    onClicked: _togglesStack.push(page)
                }

                ToggleTemplate
                {
                    icon.name: "continuous"
                }

                NetworkToggle
                {
                    onClicked: _togglesStack.push(page)
                }

                ToggleTemplate
                {
                    icon.name:  "mic-ready"
                }

                ToggleTemplate
                {
                    icon.name:  "network-flightmode-on"
                }

                ToggleTemplate
                {
                    icon.name:  "contrast"
                    text: "Dark Mode"
                }
            }




        }
    }
}
