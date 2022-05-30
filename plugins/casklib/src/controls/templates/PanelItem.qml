import QtQuick 2.15
import QtQuick.Controls 2.15

import org.mauikit.controls 1.3 as Maui

import org.maui.cask 1.0 as Cask

import QtQuick.Templates 2.15 as T

T.AbstractButton
{
    id: control

    default property alias content : _layout.data

    property int iconSize : Maui.Style.iconSizes.small

    property Cask.PanelCard card : null
    property Cask.PanelSection section : control.parent.parent
    property bool flat: win.formFactor !== Cask.Env.Desktop

    focus: true
    focusPolicy: Qt.StrongFocus

    hoverEnabled: true

    checkable: false
    checked: card ?  card.isOpen : false

    implicitWidth: implicitContentWidth + leftPadding + rightPadding
    implicitHeight: Math.max(iconSize, implicitContentHeight) + topPadding + bottomPadding //keep it fixed size

    icon.height: iconSize
    icon.width: iconSize
    icon.color: Maui.Theme.textColor

    font.bold: true
    font.weight: Font.Bold

    padding: spacing
    spacing: control.flat ? Maui.Style.space.small : Maui.Style.space.medium

    Behavior on iconSize
    {
        NumberAnimation
        {
            duration: Maui.Style.units.longDuration
            easing.type: Easing.InOutQuad
        }
    }

    Behavior on implicitWidth
    {
        NumberAnimation
        {
            duration: Maui.Style.units.shortDuration
            easing.type: Easing.OutInQuad
        }
    }

    background: Rectangle
    {
        visible: !control.flat

        readonly property color finalColor: control.Maui.Theme.backgroundColor
        color: finalColor
        radius: 6
        opacity: control.checked ?  1 : 0.8

        Rectangle
        {
            anchors.fill: parent
            color: "transparent"
            radius: parent.radius
            ColorAnimation on color
            {
                id: _animation
                easing.type: Easing.InExpo
                running: false
                from: control.Maui.Theme.highlightColor
                to: "transparent"
                duration: 1000
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

    contentItem: Row
    {
        id: _layout
        spacing: control.spacing
        clip: true

        Cask.IconLabel
        {
            visible: text.length || control.icon.name.length
            height: control.iconSize
            icon.source: control.icon.name
            icon.color: control.icon.color

            labelVisible: text.length && (control.display === ToolButton.TextBesideIcon)
            text: control.text
//            label.font: control.font
        }
    }

    function animate()
    {
        _animation.restart()
    }

    onClicked:
    {
        if(control.card && control.section)
        {
            if(card.isOpen)
            {
                control.section.close(card)
            }else
            {
                control.section.open(card)
            }
        }
    }
}
