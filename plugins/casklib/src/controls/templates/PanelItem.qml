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

    focus: true
    focusPolicy: Qt.StrongFocus
    hoverEnabled: true
    checkable: false
    checked: card ?  card.isOpen : false

    implicitWidth: implicitContentWidth + leftPadding + rightPadding
    implicitHeight: Math.max(iconSize, implicitContentHeight) + topPadding + bottomPadding //keep it fixed size

    default property alias content : _layout.data

    property int iconSize :Maui.Style.iconSizes.small

    icon.height: iconSize
    icon.width: iconSize

    leftPadding: Maui.Style.space.medium
    rightPadding: leftPadding

    padding: Maui.Style.space.small

    icon.color: control.checked || control.hovered || control.down || control.pressed ? Kirigami.Theme.highlightColor : Kirigami.Theme.textColor

    spacing: Maui.Style.space.medium
    //    checkable: true

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
        visible: !isMobile

        property color finalColor: control.checked ? Qt.darker(Kirigami.Theme.backgroundColor) : Kirigami.Theme.backgroundColor
        color: finalColor
        radius: 6

        ColorAnimation on color
        {
            id: _animation
            easing.type: Easing.InExpo
            running: false
            from: Kirigami.Theme.highlightColor
            to: _bg.finalColor
            duration: 1000
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
