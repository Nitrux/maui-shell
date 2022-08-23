import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

import org.mauikit.controls 1.2 as Maui

import org.maui.cask 1.0 as Cask
import org.mauicore.power 1.0 as CaskPower
import "../../../templates"
import org.kde.plasma.private.volume 0.1 as PA

StackPage
{
    id: control
    implicitHeight: _layout.implicitHeight + topPadding + bottomPadding + headerContainer.implicitHeight + footerContainer.implicitHeight + Maui.Style.space.big
    title: i18n("Power")
    property alias model : _listView.model

    CaskPower.PowerProfile
    {
        id: _powerProfile
    }

    Maui.Holder
    {
        anchors.fill: parent
        visible: _listView.count === 0
        title: i18n("Nothing here")
        body: i18n("No devices found.")
        emoji: "code-context"
    }

    Column
    {
        id: _layout
        spacing: Maui.Style.space.medium
        anchors.fill: parent

        Maui.ListBrowser
        {
            id: _listView
            width: parent.width
            padding: 0
            delegate: BatteryBar
            {
                battery: model.Battery
                width: ListView.view.width
                label.text: battery.prettyName + " - " + battery.type
                label2.text: battery.percent + "%"
                icon.source: batteryTypeIcon(battery.type)
            }
        }

        RowLayout
        {
            width: parent.width
            spacing: _layout.spacing

            Repeater
            {
                model: _powerProfile.profileChoices

                delegate: CardButton
                {
                    Layout.fillWidth: true
                    implicitHeight: 64+ topPadding + bottomPadding
                    text: modelData
                    icon.name: "anchor"
                    checked: _powerProfile.currentProfile === modelData
                    display: ToolButton.TextUnderIcon

                    onClicked: _powerProfile.changeProfile(modelData)
                }
            }
        }

        RowLayout
        {
            width: parent.width
            spacing: _layout.spacing

            Repeater
            {
                model: _powerProfile.profileHolds

                delegate: CardButton
                {
                    Layout.fillWidth: true

                    text: modelData.Name
                    icon.name: modelData.Icon
                    display: ToolButton.TextUnderIcon
                }
            }
        }
    }

    function batteryTypeIcon(type)
    {
        switch (type)
        {
        case "Mouse" : return "input-mouse";
        case "Keyboard" : return "input-keyboard";
        case "Battery" : return "battery-good";
        case "Monitor" : return "monitor";
        case "Pda" : return "input-mouse";
        case "Phone" : return "phone";
        case "Bluetooth" : return "bluetooth";
        case "GamingInput" : return "input-gamepad";
        }
    }
}
