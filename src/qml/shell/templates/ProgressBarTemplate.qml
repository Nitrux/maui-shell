import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Templates 2.15 as T

import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.2 as Maui

T.ProgressBar
{
    id: control
    implicitHeight: 22

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

    }


    contentItem: Rectangle
    {

        implicitWidth: control.height
        implicitHeight: control.height
        color: Kirigami.Theme.highlightColor
        radius: height/2
        Kirigami.Icon
        {
            id: _icon
            height: 16
            width : 16
            color: Kirigami.Theme.highlightedTextColor
            anchors.centerIn: parent
        }
    }
}
