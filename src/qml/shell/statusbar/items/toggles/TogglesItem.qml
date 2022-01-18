import QtQuick 2.15
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5

import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.2 as Maui

import org.maui.cask 1.0 as Cask
import "sections/network"
import "sections/bluetooth"
import "sections/screenshot"

Cask.PanelItem
{
    id: control

    Row
    {
        spacing: control.spacing       

        Kirigami.Icon
        {
            source: "network-wireless"
            height: control.iconSize
            width: height
            color: control.icon.color
        }        

        Kirigami.Icon
        {
            source: "network-bluetooth"
            height: control.iconSize
            width: height
            color: control.icon.color
        }

        Kirigami.Icon
        {
            source: "settings-configure"
            height: control.iconSize
            width: height
            color: control.icon.color
        }
    }

    card: Cask.PanelCard
    {
        width: parent.width

        onClosed: _togglesStack.pop()

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
                NetworkToggle
                {
                    onClicked: _togglesStack.push(page)
                    width:  Math.floor(parent.width/2) - parent.spacing
                }

                BluetoothToggle
                {
                    onClicked: _togglesStack.push(page)
                    width:  Math.floor(parent.width/2)- parent.spacing

                }

                ToggleTemplate
                {
                    icon.name: _slidersItem.micItem.slider.iconSource
                    text: "Michrophone"
                    checked: _slidersItem.micItem.muted
                    onClicked: _slidersItem.micItem.toggleMute()
                }

                AirplaneModeToggle
                {

                }

                ScreenshotToggle
                {
                    id: _screenshotToggle

                    onClicked:
                    {
                        _togglesStack.push(page)
                    }
                }

                ToggleTemplate
                {
                    icon.name:  "contrast"
                    text: i18n("Nigh mode")
                }

                ToggleTemplate
                {
                    icon.name:  "settings-configure"
                    text: i18n("Settings")
                }
            }

        }
    }
}
