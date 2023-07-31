import QtQuick
import QtQuick.Controls

import org.mauikit.controls as Maui
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM

import org.maui.cask as Cask
import "../.."

ToggleTemplate
{
    icon.name: connectionIconProvider.connectionIcon
    text: networkStatus.activeConnections.split(" ").pop()
    tooltipText: networkStatus.activeConnections
    display: ToolButton.TextBesideIcon

    property alias handler : handler

    BusyIndicator
    {
        id: connectingIndicator

        anchors.centerIn: parent
        height: Math.min (parent.width, parent.height)
        width: height
        running: connectionIconProvider.connecting
    }

    page: NetworkPage
    {

    }

    PlasmaNM.NetworkStatus
    {
        id: networkStatus
    }

    PlasmaNM.ConnectionIcon
    {
        id: connectionIconProvider
    }

    PlasmaNM.Handler
    {
        id: handler
    }
}
