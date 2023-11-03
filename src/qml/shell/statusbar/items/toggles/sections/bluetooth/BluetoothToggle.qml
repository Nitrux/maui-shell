import QtQuick
import QtQuick.Controls

import org.mauikit.controls as Maui

import org.maui.cask as Cask
import org.kde.bluezqt as BluezQt

import org.kde.plasma.private.bluetooth as PlasmaBt

import "../.."
import "logic.js" as Logic


ToggleTemplate
{
    id: control
    icon.name: control.iconName
    text: control.tooltipText
    display: ToolButton.TextBesideIcon

    property string iconName: Logic.icon()
    property bool deviceConnected : false
    property int runningActions : 0
    property string tooltipText
    property int status

    property QtObject btManager : BluezQt.Manager

    BusyIndicator
    {
        anchors.centerIn: parent
        running: control.runningActions > 0
    }

    page: BluetoothPage
    {
        id: _bluetoothPage
    }

    PlasmaBt.DevicesProxyModel
    {
        id: devicesModel
        sourceModel: BluezQt.DevicesModel { }
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

    Component.onCompleted:
    {
        //           plasmoid.removeAction("configure");
        //           plasmoid.setAction("configure", i18n("Configure &Bluetooth…"), "configure");

        //           plasmoid.setAction("addNewDevice", i18n("Add New Device…"), "list-add");
        //           plasmoid.action("addNewDevice").visible = Qt.binding(() => {return });

        Logic.updateStatus();
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

}
