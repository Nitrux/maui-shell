import QtQuick 2.15
import QtQuick.Controls 2.15

import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.2 as Maui

import org.maui.cask 1.0 as Cask

import org.kde.bluezqt 1.0 as BluezQt
import org.kde.plasma.private.bluetooth 1.0 as PlasmaBt

import "logic.js" as Logic

import "../../../../../templates"

StackPage
{
    id: control

    property bool deviceConnected : false
    property int runningActions : 0

    property QtObject btManager : BluezQt.Manager

    implicitHeight: Math.max(400, Math.min(400, _listView.contentHeight + topPadding + bottomPadding + headerContainer.implicitHeight + footerContainer.implicitHeight + Maui.Style.space.big))
    title: i18n("Bluetooth")
    property string tooltipText
    property int status
    Action {
        id: addBluetoothDeviceAction

        enabled: btManager.devices.length === 0

        text: i18n("Add")
        icon.name: "list-add"

        onTriggered: action_addNewDevice()
    }

    Action {
        id: enableBluetoothAction

        //        enabled: btManager.bluetoothBlocked

        //        text: i18n("Enable Bluetooth")
        //        icon.name: "preferences-system-bluetooth"

        onTriggered: {
            toggleBluetooth();
        }
    }


    Connections
    {
        target: btManager

        function onDeviceAdded() {
            Logic.updateStatus();
        }
        function onDeviceRemoved() {
            Logic.updateStatus();
        }
        function onDeviceChanged() {
            Logic.updateStatus();
        }
        function onBluetoothBlockedChanged() {
            Logic.updateStatus();
        }
        function onBluetoothOperationalChanged() {
            Logic.updateStatus();
        }
    }

    headBar.rightContent: [
        ToolButton
        {
            icon.name: "list-add"
            enabled: !btManager.bluetoothBlocked;
            onClicked: action_addNewDevice()
        },
        Switch
        {
            checkable: true
            checked: btManager.bluetoothOperational
            action: enableBluetoothAction
        }
    ]

    Maui.ListBrowser
    {
        id: _listView
        anchors.fill: parent
        padding: 0
        holder.visible: _listView.count === 0
        holder.title: i18n("Nothing here")
        holder.body: {
            // We cannot use the adapter count here because that can be zero when
            // bluetooth is disabled even when there are physical devices
            if (BluezQt.Manager.rfkill.state === BluezQt.Rfkill.Unknown) {
                return i18n("No Bluetooth Adapters Available")
            } else if (btManager.bluetoothBlocked) {
                return i18n("Bluetooth is Disabled")
            } else if (btManager.devices.length === 0) {
                return i18n("No Devices Found")
            }
            return ""
        }

        holder.emoji: "code-context"

        model: btManager.adapters.length > 0 && !btManager.bluetoothBlocked ? devicesModel : null
        currentIndex: -1

        delegate: ConnectionDelegate
        {
            width: ListView.view.width
        }
    }

    PlasmaBt.DevicesProxyModel
    {
        id: devicesModel
        sourceModel: BluezQt.DevicesModel { }
    }


    function toggleBluetooth()
    {
        var enable = !btManager.bluetoothOperational;
        btManager.bluetoothBlocked = !enable;

        for (var i = 0; i < btManager.adapters.length; ++i)
        {
            var adapter = btManager.adapters[i];
            adapter.powered = enable;
        }
    }

    function action_configure() {
        KCMShell.openSystemSettings("kcm_bluetooth");
    }

    function action_addNewDevice() {
        PlasmaBt.LaunchApp.runCommand("bluedevil-wizard");
    }

    Component.onCompleted:
    {
        //           plasmoid.removeAction("configure");
        //           plasmoid.setAction("configure", i18n("Configure &Bluetooth…"), "configure");

        //           plasmoid.setAction("addNewDevice", i18n("Add New Device…"), "list-add");
        //           plasmoid.action("addNewDevice").visible = Qt.binding(() => {return });

        Logic.updateStatus();
    }

}
