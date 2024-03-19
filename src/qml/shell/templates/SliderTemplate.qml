import QtQuick
import QtQuick.Controls

import org.mauikit.controls as Maui

import Qt5Compat.GraphicalEffects

Slider
{
    id: control

    Maui.Theme.colorSet: Maui.Theme.Button
    Maui.Theme.inherit: false

    implicitHeight: 16 + topPadding + bottomPadding
    opacity: enabled ? 1 : 0.7

    property alias iconSource : _icon.source
    property alias animatedRec : _animatedRec

    Behavior on implicitHeight
    {
        NumberAnimation
        {
            duration: Maui.Style.units.shortDuration
            easing.type: Easing.OutQuad
        }
    }

    background: Rectangle
    {
        radius: height/2
        color: Qt.darker(Maui.Theme.backgroundColor, 1.2)

        Rectangle
        {
            id: _bg

            width: Math.max(height, control.visualPosition * parent.width)
            height: control.height
            anchors.verticalCenter: parent.verticalCenter
            color: control.enabled ? Maui.Theme.highlightColor : "transparent"
            border.color: control.enabled ? "transparent" : Maui.Theme.highlightColor
            radius: height/2
            clip: true

            Rectangle
            {
                id: _animatedRec
                visible: control.enabled
                height: parent.height
                color: Qt.lighter(parent.color)
                opacity: 0.5
                Behavior on width
                {
                    NumberAnimation
                    {
                        id: animation
                        duration: Maui.Style.units.shortDuration
                        easing.type: Easing.OutQuad
                    }
                }
            }

            Behavior on color
            {
                ColorAnimation
                {
                    easing.type: Easing.InQuad
                    duration: Maui.Style.units.shortDuration
                }
            }

            layer.enabled: _animatedRec.visible
            layer.effect: OpacityMask
            {
                maskSource: Rectangle
                {
                    width: _bg.width
                    height: _bg.height
                    radius: _bg.radius
                }
            }
        }

        Behavior on color
        {
            ColorAnimation
            {
                easing.type: Easing.InQuad
                duration: Maui.Style.units.shortDuration
            }
        }
    }

    handle: Rectangle
    {
        x: Math.max(0,(control.visualPosition * parent.availableWidth) - width)
        y: control.topPadding + control.availableHeight / 2 - height / 2

        implicitWidth: control.height
        implicitHeight: control.height

        color: control.enabled ? Maui.Theme.highlightColor : "transparent"
        radius: height/2

        Maui.Icon
        {
            id: _icon
            height: 16
            width : 16
            isMask: true
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
