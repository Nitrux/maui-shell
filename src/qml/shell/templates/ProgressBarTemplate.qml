import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Templates 2.15 as T

import org.mauikit.controls 1.3 as Maui

import org.maui.cask 1.0 as Cask

T.ProgressBar
{
    id: control

    Maui.Theme.colorSet: Maui.Theme.Button
    Maui.Theme.inherit: false

    implicitHeight: 16 + topPadding + bottomPadding
    opacity: enabled ? 1 : 0.7

    property alias iconSource : _icon.source
    property color color : Maui.Theme.highlightColor

    Behavior on color
    {
        ColorAnimation
        {
            easing.type: Easing.InQuad
            duration: Maui.Style.units.shortDuration
        }
    }

    background: Rectangle
    {
        radius: height/2
        color: Qt.darker(Maui.Theme.backgroundColor, 1.2)

        Behavior on color
        {
            ColorAnimation
            {
                easing.type: Easing.InQuad
                duration: Maui.Style.units.shortDuration
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

        Maui.Icon
        {
            id: _icon
            height: 16
            width : 16
            color: Maui.Theme.highlightedTextColor
            anchors.centerIn: parent
            Behavior on color
            {
                ColorAnimation
                {
                    easing.type: Easing.InQuad
                    duration: Maui.Style.units.shortDuration
                }
            }
        }
    }
}
