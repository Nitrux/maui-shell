import QtQuick
import QtQuick.Controls

import org.mauikit.controls as Maui

import org.maui.cask as Cask

import org.kde.plasma.networkmanagement as PlasmaNM

import "../../../../../templates"

StackPage
{
    id: control

    // The PlasmaNM.Handler lives in NetworkToggle.qml. QML ids are file-local,
    // so it has to be handed over explicitly instead of being read from the
    // toggle's scope.
    property var handler

    implicitHeight: Math.max(400, Math.min(400, _listView.contentHeight + topPadding + bottomPadding + headerContainer.implicitHeight + footerContainer.implicitHeight + Maui.Style.space.big))
    title: i18n("Networks")

    readonly property var displayWifiMessage: !wifiSwitchButton.checked && wifiSwitchButton.visible
    readonly property var displayWwanMessage: !wwanSwitchButton.checked && wwanSwitchButton.visible
    readonly property var displayplaneModeMessage: PlasmaNM.Configuration.airplaneModeEnabled

    headBar.rightContent: [
        Switch
        {
            id: wifiSwitchButton
            checkable: true
            checked: enabled && enabledConnections.wirelessEnabled
            enabled: enabledConnections.wirelessHwEnabled && availableDevices.wirelessDeviceAvailable && !PlasmaNM.Configuration.airplaneModeEnabled
            visible: availableDevices.wirelessDeviceAvailable

            onToggled: handler.enableWireless(checked);
        },

        Switch
        {
            id: wwanSwitchButton

            checked: enabled && enabledConnections.wwanEnabled
            enabled: enabledConnections.wwanHwEnabled && availableDevices.modemDeviceAvailable && !PlasmaNM.Configuration.airplaneModeEnabled

            icon.name: enabled ? "network-mobile-on" : "network-mobile-off"
            visible: availableDevices.modemDeviceAvailable

            onToggled: handler.enableWwan(checked);

        }
    ]

    PlasmaNM.EnabledConnections
    {
        id: enabledConnections

        onWirelessEnabledChanged: {
            wifiSwitchButton.checked = wifiSwitchButton.enabled && enabled
        }

        onWirelessHwEnabledChanged: {
            wifiSwitchButton.enabled = enabled && availableDevices.wirelessDeviceAvailable && !PlasmaNM.Configuration.airplaneModeEnabled
        }

        onWwanEnabledChanged: {
            wwanSwitchButton.checked = wwanSwitchButton.enabled && enabled
        }

        onWwanHwEnabledChanged: {
            wwanSwitchButton.enabled = enabled && availableDevices.modemDeviceAvailable && !PlasmaNM.Configuration.airplaneModeEnabled
        }
    }

    PlasmaNM.AvailableDevices
    {
        id: availableDevices
    }

    PlasmaNM.NetworkModel
    {
        id: connectionModel
    }

    PlasmaNM.AppletProxyModel
    {
        id: appletProxyModel

        sourceModel: connectionModel
    }

    Timer
    {
        id: scanTimer
        interval: 10200
        repeat: true
        running: control.visible && !connectionIconProvider.airplaneMode

        onTriggered: handler.requestScan()
    }

    Maui.InputDialog
    {
        id: _credentialsDialog
        property var connection : null

        title: connection ? i18n("Connect to %1", connection.ItemUniqueName) : i18n("Connect")
        message: i18n("Enter credentials to connect to network")

        onFinished: (text) =>
        {
            if(connection)
                handler.addAndActivateConnection(connection.DevicePath, connection.SpecificPath, text)
        }
    }

    Maui.ListBrowser
    {
        id: _listView
        anchors.fill: parent
        padding: 0
        model: appletProxyModel
        currentIndex: -1
        holder.title: i18n("Networks")
        holder.visible: count === 0
        holder.body: {
            if (control.displayplaneModeMessage) {
                return i18n("Airplane mode is enabled")
            }
            if (control.displayWifiMessage) {
                if (control.displayWwanMessage) {
                    return i18n("Wireless and mobile networks are deactivated")
                }
                return i18n("Wireless is deactivated")
            }
            if (control.displayWwanMessage) {
                return i18n("Mobile network is deactivated")
            }
            return i18n("No available connections")
        }

        delegate: ConnectionDelegate
        {
            width: ListView.view.width
            checked: ConnectionState == PlasmaNM.Enums.Activated
            label1.text: model.ItemUniqueName
            label1.font.bold: true
            label1.font.pointSize: Maui.Style.fontSizes.big
            label2.text: itemText()
            iconSource: model.ConnectionIcon
            onClicked:
            {
                if (Uuid || !predictableWirelessPassword)
                {
                    if (ConnectionState == PlasmaNM.Enums.Deactivated)
                    {
                        if (!predictableWirelessPassword && !Uuid)
                        {
                            handler.addAndActivateConnection(DevicePath, SpecificPath)
                        } else {
                            handler.activateConnection(ConnectionPath, DevicePath, SpecificPath)
                        }
                    } else {
                        handler.deactivateConnection(ConnectionPath, DevicePath)
                    }
                } else if (predictableWirelessPassword) {
                    _credentialsDialog.connection = model
                    _credentialsDialog.open()
                }
            }
        }
    }

    Component.onCompleted:
    {
        handler.requestScan();
    }
}
