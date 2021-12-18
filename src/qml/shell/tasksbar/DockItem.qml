import QtQuick 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5

import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.2 as Maui
import org.maui.cask 1.0 as Cask

Cask.PanelItem
{
    id: control
    implicitHeight: 48
    implicitWidth: 48

    leftPadding: 0
    rightPadding: 0

    background: Rectangle
    {
        color: Qt.darker(Kirigami.Theme.backgroundColor)
        opacity: control.checked? 1 : 0.7
        radius: 10
    }

    contentItem: Item
    {

        Kirigami.Icon
        {
            source: control.icon.name
            color: control.checked || control.down ? Kirigami.Theme.highlightColor : Kirigami.Theme.textColor
            height: 22
            width: height
            anchors.centerIn: parent
        }
    }
}
