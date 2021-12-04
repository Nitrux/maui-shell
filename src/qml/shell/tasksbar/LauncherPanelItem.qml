import QtQuick 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5

import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.2 as Maui
import org.maui.cask 1.0 as Cask

Cask.PanelItem
{
    id: control
    iconSize: 48
    leftPadding: 0
    rightPadding: 0

    background: Rectangle
    {
        color: Qt.darker(Kirigami.Theme.backgroundColor)
        opacity: control.card.visible ? 1 : 0.7
        radius: 10
    }

    Item
    {
        Layout.preferredHeight: control.iconSize
        Layout.preferredWidth: height

        Kirigami.Icon
        {
            source: "view-list-icons"
            color: Kirigami.Theme.textColor
            height: 22
            width: height
            anchors.centerIn: parent
        }
    }

    card: Cask.PanelCard
    {
        width: ListView.view.width
        implicitHeight: Math.min(_cask.avaliableHeight, 800)
        background: Item{}

        onOpened:
        {
console.log("CARD OPENED")
                _launcherGrid.forceActiveFocus()

        }

        Launcher
        {
            id: _launcherGrid
            height: parent.height
            width: Math.min(800, parent.width)

            //           scale: parent.opacity
        }
    }
}
