import QtQuick 2.15
import QtQuick.Controls 2.15

import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.2 as Maui

import org.maui.cask 1.0 as Cask
import org.kde.bluezqt 1.0 as BluezQt
import org.kde.plasma.private.bluetooth 1.0 as PlasmaBt

import "../.."

TogglePage
{
    id: control

    implicitHeight: Math.max(400, Math.min(400, _listView.contentHeight + topPadding + bottomPadding + headerContainer.implicitHeight + footerContainer.implicitHeight + Maui.Style.space.big))
    headerColorSet: control.Kirigami.Theme.colorSet
    title: "Networks"
    showTitle: true

    headBar.rightContent: [
        Switch
        {

            checkable: true

        }
    ]

    Maui.ListBrowser
    {
        id: _listView
        anchors.fill: parent
    }

}
