import QtQuick 2.13
import QtQml 2.14
import QtQuick.Window 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.13
import QtGraphicalEffects 1.0

import org.kde.kirigami 2.7 as Kirigami
import org.mauikit.controls 1.2 as Maui
import org.maui.cask 1.0 as Cask
import QtQuick.Templates 2.15 as T

T.AbstractButton
{
    id: control
    property PanelCard card : null
    focus: true
    focusPolicy: Qt.StrongFocus
    hoverEnabled: true

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(iconSize+ Maui.Style.space.medium, 16) + topPadding + bottomPadding //keep it fixed size

    default property alias content : _layout.data

    property int iconSize :Maui.Style.iconSizes.small

    icon.height: iconSize
    icon.width: iconSize

    leftPadding: isMobile ? 0 : Maui.Style.space.medium
    rightPadding: leftPadding

    icon.color: control.checked || control.hovered || control.down || control.pressed ? Kirigami.Theme.highlightColor : Kirigami.Theme.textColor

    checked: card ? card.visible : false
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
        visible: !isMobile
        color: control.checked ? Qt.darker(Kirigami.Theme.backgroundColor) : Kirigami.Theme.backgroundColor
        radius: 6
    }

    contentItem: RowLayout
    {
        id: _layout
        spacing: Maui.Style.space.small
        clip: true

        Item
        {
            visible: control.icon.name.length
            Layout.preferredWidth: height
            implicitHeight: Math.min(control.iconSize+ Maui.Style.space.medium, control.height)

            Kirigami.Icon
            {
                id: _icon
                source: control.icon.name
                height: control.icon.height
                width: control.icon.width
                anchors.centerIn: parent
                color: control.icon.color
            }
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

}
