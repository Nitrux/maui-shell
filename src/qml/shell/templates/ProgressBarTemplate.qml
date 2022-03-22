import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Templates 2.15 as T

import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.2 as Maui

import org.maui.cask 1.0 as Cask

T.ProgressBar
{
    id: control
    implicitHeight: (win.formFactor === Cask.Env.Desktop ? 16 : 22 ) + topPadding + bottomPadding

    property alias iconSource : _icon.source
    property color color : Kirigami.Theme.highlightColor

    background: Rectangle
    {
        radius: height/2
        color: control.enabled ? Qt.darker(Kirigami.Theme.backgroundColor, 1.2) : "transparent"
        border.color: control.enabled ? "transparent" : Qt.darker(Kirigami.Theme.backgroundColor, 1.2)

        Behavior on color
        {
            ColorAnimation
            {
                easing.type: Easing.InQuad
                duration: Kirigami.Units.longDuration
            }
        }
    }

    contentItem: Item
    {
        implicitWidth: control.height
        implicitHeight: control.height

        Rectangle
        {
            height: parent.height

            width: control.visualPosition * parent.width
            color: control.color
            radius: height/2
        }

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
