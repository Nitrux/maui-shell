import QtQuick 2.15
import QtQuick.Controls 2.5

import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.2 as Maui
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM
import org.kde.kcoreaddons 1.0 as KCoreAddons

Maui.ListBrowserDelegate
{
    id: control
    property real rxBytes: 0
    property real txBytes: 0


    property alias isBusy : _busyIndicator.running
    property bool showSpeed: ConnectionState == PlasmaNM.Enums.Activated &&
                             (Type == PlasmaNM.Enums.Wired ||
                              Type == PlasmaNM.Enums.Wireless ||
                              Type == PlasmaNM.Enums.Gsm ||
                              Type == PlasmaNM.Enums.Cdma)
    property bool activating: ConnectionState == PlasmaNM.Enums.Activating
        property bool deactivated: ConnectionState === PlasmaNM.Enums.Deactivated
        property bool passwordIsStatic: (SecurityType == PlasmaNM.Enums.StaticWep || SecurityType == PlasmaNM.Enums.WpaPsk ||
                                         SecurityType == PlasmaNM.Enums.Wpa2Psk || SecurityType == PlasmaNM.Enums.SAE)
        property bool predictableWirelessPassword: !Uuid && Type == PlasmaNM.Enums.Wireless && passwordIsStatic

    tooltipText: label2.text
    iconSizeHint: 16
    iconVisible: !control.checked

    onShowSpeedChanged: {
            connectionModel.setDeviceStatisticsRefreshRateMs(DevicePath, showSpeed ? 2000 : 0)
        }


    onActivatingChanged: {
        if (ConnectionState == PlasmaNM.Enums.Activating) {
            ListView.view.positionViewAtBeginning()
        }
    }


//    onDeactivatedChanged: {
//        /* Separator is part of section, which is visible only when available connections exist. Need to determine
//           if there is a connection in use, to show Separator. Otherwise need to hide it from the top of the list.
//           Connections in use are always on top, only need to check the first one. */
//        if (appletProxyModel.data(appletProxyModel.index(0, 0), PlasmaNM.NetworkModel.SectionRole) !== "Available connections") {
//            if (connectionView.showSeparator != true) {
//                connectionView.showSeparator = true
//            }
//            return
//        }
//        connectionView.showSeparator = false
//        return
//    }


//    Kirigami.Icon
//    {
//        implicitHeight: 16
//        implicitWidth: implicitHeight
//        source: "object-locked"
//        color: Kirigami.Theme.textColor
//    }

    BusyIndicator
    {
        id: _busyIndicator
        running: control.activating
        implicitHeight: visible ? 22 : 0
        implicitWidth: implicitHeight
    }

    Timer {
           id: timer
           repeat: true
           interval: 2000
           running: showSpeed
           triggeredOnStart: true
           property real prevRxBytes
           property real prevTxBytes
           Component.onCompleted: {
               prevRxBytes = 0
               prevTxBytes = 0
           }
           onTriggered: {
               rxBytes = prevRxBytes == 0 ? 0 : (RxBytes - prevRxBytes) * 1000 / interval
               txBytes = prevTxBytes == 0 ? 0 : (TxBytes - prevTxBytes) * 1000 / interval
               prevRxBytes = RxBytes
               prevTxBytes = TxBytes
           }
       }


    function itemText() {
        if (ConnectionState == PlasmaNM.Enums.Activating) {
            if (Type == PlasmaNM.Enums.Vpn)
                return VpnState
            else
                return DeviceState
        } else if (ConnectionState == PlasmaNM.Enums.Deactivating) {
            if (Type == PlasmaNM.Enums.Vpn)
                return VpnState
            else
                return DeviceState
        } else if (Uuid && ConnectionState == PlasmaNM.Enums.Deactivated) {
            return LastUsed
        } else if (ConnectionState == PlasmaNM.Enums.Activated) {
            if (showSpeed) {
                return i18n("⬇ %1/s, ⬆ %2/s",
                            KCoreAddons.Format.formatByteSize(rxBytes),
                            KCoreAddons.Format.formatByteSize(txBytes))
            } else {
                return i18n("Connected")
            }
        }
        return ""
    }



}
