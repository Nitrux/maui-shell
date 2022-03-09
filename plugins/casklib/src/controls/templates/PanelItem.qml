import QtQuick 2.15
import QtQuick.Controls 2.15

import QtQuick.Layouts 1.3

import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.3 as Maui

import org.maui.cask 1.0 as Cask
import QtQuick.Templates 2.15 as T

T.AbstractButton
{
    id: control
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

    default property alias content : _layout.data

    property int iconSize : Maui.Style.iconSizes.small

    icon.height: iconSize
    icon.width: iconSize

    padding: spacing

    icon.color: Kirigami.Theme.textColor

    spacing: control.flat ? Maui.Style.space.small : Maui.Style.space.medium

    Behavior on iconSize
    {
        NumberAnimation
        {
            duration: Kirigami.Units.longDuration
            easing.type: Easing.InOutQuad
        }
    }

    background: Rectangle
    {
        id: _bg
        visible: !control.flat

       readonly property color finalColor: Kirigami.Theme.backgroundColor
        color: finalColor
        radius: 6
        opacity: control.checked ?  1 : 0.8

        ColorAnimation on color
        {
            id: _animation
            easing.type: Easing.InExpo
            running: false
            from: Kirigami.Theme.highlightColor
            to: _bg.finalColor
            duration: 1000
        }

        Behavior on color
        {
            ColorAnimation
            {
                easing.type: Easing.InQuad
                duration: Kirigami.Units.longDuration
            }
        }
    }

    contentItem: RowLayout
    {
        id: _layout
        spacing: control.spacing
        clip: true

        Kirigami.Icon
        {
            id: _icon
            visible: control.icon.name.length
            Layout.preferredWidth: height
            Layout.alignment: Qt.AlignCenter
            implicitHeight: control.iconSize

            source: control.icon.name

            color: control.icon.color
        }

        Label
        {
            visible: text.length && (control.display === ToolButton.TextBesideIcon)
            Layout.preferredWidth: implicitWidth
            text: control.text
            horizontalAlignment: Qt.AlignLeft
            verticalAlignment: Qt.AlignVCenter
            wrapMode: Text.NoWrap
            elide: Text.ElideRight
            color: _icon.color
            font: control.font
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
