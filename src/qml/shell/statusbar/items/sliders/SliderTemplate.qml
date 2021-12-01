import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Templates 2.15 as T

import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.2 as Maui

T.Slider
{
    id: control
    implicitHeight: 32

    property alias iconSource : _icon.source

    background: Rectangle
    {
        x: control.leftPadding
        y: control.topPadding + control.availableHeight / 2 - height / 2
        implicitWidth: 200
        implicitHeight: 4
        width: control.availableWidth
        height: implicitHeight
        radius: height/2
        color: Qt.darker(Kirigami.Theme.backgroundColor, 1.2)

        Rectangle
        {
            width: Math.max(height, control.visualPosition * parent.width)
            height: 32
            anchors.verticalCenter: parent.verticalCenter
            color: Kirigami.Theme.highlightColor
            radius: height/2


        }
    }

    handle: Rectangle
    {
        x: Math.max(0,(control.visualPosition * parent.availableWidth) - width)
        y: control.topPadding + control.availableHeight / 2 - height / 2
        implicitWidth: 32
        implicitHeight: 32
        color: Kirigami.Theme.highlightColor
        radius: 16
        Kirigami.Icon
        {
            id: _icon
            height: 22
            width : 22
            color: Kirigami.Theme.highlightedTextColor
            anchors.centerIn: parent
        }
    }
}
