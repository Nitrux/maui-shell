import QtQuick 2.15
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5
import QtQuick.Templates 2.15 as T

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

        StackView
        {
            id : _togglesStack
            width: parent.width
            height: currentItem.implicitHeight
            clip: true
            padding: 0

            initialItem: Item
            {
                implicitHeight: 140
                GridLayout
            {
                anchors.fill: parent
                id:_tooglesGrid
                rows: 3
                columns: 5

//                spacing: Maui.Style.space.medium

                NetworkToggle
                {
                    id: _networkToggle
                    onClicked: _togglesStack.push(page)
                    Layout.fillWidth: true
                    Layout.columnSpan: 2
                    Layout.fillHeight: true
                }

                BluetoothToggle
                {
                    id: _bluetoothToggle
                    onClicked: _togglesStack.push(page)
                    Layout.fillWidth: true
                    Layout.columnSpan: 2
                    Layout.rowSpan: 2
                    Layout.fillHeight: true
//                    display: ToolButton.TextUnderIcon
                }

                ToggleTemplate
                {
                    icon.name: _slidersItem.micItem.slider.iconSource
                    text: "Michrophone"
                    checked: !_slidersItem.micItem.muted
                    onClicked: _slidersItem.micItem.toggleMute()
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }

                AirplaneModeToggle
                {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }

                ScreenshotToggle
                {
                    id: _screenshotToggle
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    onClicked:
                    {
                        _togglesStack.push(page)
                    }
                }

                DevicesToggle
                {
                    onClicked: _togglesStack.push(page)
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }

                ToggleTemplate
                {
                    icon.name:  "contrast"
                    enabled: Maui.Style.styleType !== Maui.Style.Adaptive
                    text: i18n("Nigh mode")
                    checked: Maui.Style.styleType === Maui.Style.Dark
                    onClicked: Maui.Style.styleType = (Maui.Style.styleType === Maui.Style.Dark ? Maui.Style.Light : Maui.Style.Dark)
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }

                ToggleTemplate
                {
                    icon.name:  "settings-configure"
                    text: i18n("Settings")
                    onClicked: Cask.MauiMan.invokeManager("background")
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
            }
            }
        }
    }
}
