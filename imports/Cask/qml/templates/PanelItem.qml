import QtQuick 2.13
import QtQml 2.14
import QtQuick.Window 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.13
import QtGraphicalEffects 1.0

import org.kde.kirigami 2.7 as Kirigami
import org.kde.mauikit 1.2 as Maui

AbstractButton
{
    id: control
    property PanelCard card : null
    hoverEnabled: true
    implicitWidth: _layout.implicitWidth
    implicitHeight: Maui.Style.iconSizes.medium + Maui.Style.space.small
    background: Item {}

    icon.height: Maui.Style.iconSizes.medium
    icon.width: Maui.Style.iconSizes.medium

     RowLayout
    {
        id: _layout
        anchors.centerIn: parent
        height: parent.height
        spacing: 0
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
