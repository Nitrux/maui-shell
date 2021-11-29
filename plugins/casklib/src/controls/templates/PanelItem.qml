import QtQuick 2.13
import QtQml 2.14
import QtQuick.Window 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.13
import QtGraphicalEffects 1.0

import org.kde.kirigami 2.7 as Kirigami
import org.mauikit.controls 1.2 as Maui

AbstractButton
{
    id: control
    property PanelCard card : null
    hoverEnabled: true
    implicitWidth: _layout.implicitWidth
    implicitHeight: Math.max(Maui.Style.iconSizes.medium, iconSize)

    background: Item {}

    default property alias content : _layout.data

    property int iconSize : isMobile ? Maui.Style.iconSizes.small : Maui.Style.iconSizes.medium

    icon.height: iconSize
    icon.width: iconSize

    Behavior on iconSize
    {
        NumberAnimation
        {
            duration: Kirigami.Units.longDuration
            easing.type: Easing.InOutQuad
        }
    }

    contentItem :RowLayout
    {
        id: _layout
        spacing: Maui.Style.space.medium
        clip: true

        Item
        {
            visible: control.icon.name.length
            Layout.fillHeight: true
            Layout.preferredWidth: height
            Kirigami.Icon
            {
                id: _icon
                source: control.icon.name
                height: control.icon.height
                width: control.icon.width
                anchors.centerIn: parent
                color: control.hovered || control.down || control.pressed ? Kirigami.Theme.highlightColor : Kirigami.Theme.textColor
            }
        }

        Label
        {
            visible: text.length && (control.display === ToolButton.TextBesideIcon)
            Layout.fillHeight: true
            Layout.preferredWidth: implicitWidth
            text: control.text
            horizontalAlignment: Qt.AlignLeft
            verticalAlignment: Qt.AlignVCenter
            wrapMode: Text.NoWrap
            elide: Text.ElideRight
            color: _icon.color
        }
    }
}
