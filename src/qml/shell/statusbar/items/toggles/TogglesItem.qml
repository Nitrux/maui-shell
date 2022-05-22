import QtQuick 2.15
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5
import QtQuick.Templates 2.15 as T

import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.2 as Maui

import org.maui.cask 1.0 as Cask
import "sections/network"
import "sections/bluetooth"
import "sections/screenshot"
import "sections/devices"

Cask.PanelItem
{
    id: control

    Cask.IconLabel
    {
        height: control.iconSize
        icon.source: _networkToggle.icon.name
        icon.color: control.icon.color
        text: _networkToggle.text
        labelVisible: labelsVisible
    }

    Cask.IconLabel
    {
        visible: _bluetoothToggle.deviceConnected
        icon.source: _bluetoothToggle.icon.name
        icon.color: control.icon.color
        text: _bluetoothToggle.text
        labelVisible: labelsVisible
        height: control.iconSize
    }

    Cask.IconLabel
    {
        icon.source: "settings-configure"
        height: control.iconSize
        icon.color: control.icon.color
        labelVisible: labelsVisible
    }

    card: Cask.PanelCard
    {
        width: parent.width

        onClosed: _togglesStack.pop()

        T.StackView
        {
            id : _togglesStack
            width: parent.width
            height: currentItem.implicitHeight
            clip: true
            padding: 0

            initialItem: Flow
            {
                id:_tooglesGrid

                spacing: Maui.Style.space.medium

                NetworkToggle
                {
                    id: _networkToggle
                    onClicked: _togglesStack.push(page)
                    width:  Math.floor(parent.width/2) - parent.spacing
                }

                BluetoothToggle
                {
                    id: _bluetoothToggle
                    onClicked: _togglesStack.push(page)
                    width:  Math.floor(parent.width/2)- parent.spacing

                }

                ToggleTemplate
                {
                    icon.name: _slidersItem.micItem.slider.iconSource
                    text: "Michrophone"
                    checked: !_slidersItem.micItem.muted
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

                DevicesToggle
                {
                    onClicked: _togglesStack.push(page)
                }

                ToggleTemplate
                {
                    icon.name:  "contrast"
                    enabled: !Maui.Style.adaptiveColorScheme
                    text: i18n("Nigh mode")
                    checked: Maui.App.darkMode
                    onClicked: Maui.App.darkMode = !Maui.App.darkMode
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
