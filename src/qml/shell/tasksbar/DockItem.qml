import QtQuick 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5

import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.2 as Maui
import org.maui.cask 1.0 as Cask
import "../templates"
import QtQuick.Templates 2.15 as T

T.AbstractButton
{
    id: control
    implicitHeight: 48
    implicitWidth: 48

    leftPadding: 0
    rightPadding: 0

    icon.height: 22
    icon.width: 22

    property bool colorize: false

    background: Rectangle
    {
        readonly property color m_color : control.colorize ? _imgColors.dominant : Qt.tint(Qt.lighter(control.Kirigami.Theme.textColor), Qt.rgba(control.Kirigami.Theme.backgroundColor.r, control.Kirigami.Theme.backgroundColor.g, control.Kirigami.Theme.backgroundColor.b, 0.9))

        color: control.pressed || control.down || control.checked ? control.Kirigami.Theme.highlightColor : (control.highlighted || control.hovered ? control.Kirigami.Theme.hoverColor : m_color)

        opacity: control.checked? 1 : 0.7
        radius: 8

        Kirigami.ImageColors
        {
            id: _imgColors
            source: control.icon.name
        }

        Behavior on color
        {
            ColorAnimation
            {
                easing.type: Easing.InQuad
                duration: Kirigami.Units.shortDuration
            }
        }
    }

    contentItem: Item
    {
        Kirigami.Icon
        {
            source: control.icon.name
            color: control.checked || control.down ? Kirigami.Theme.highlightedTextColor : Kirigami.Theme.textColor
            height: control.icon.height
            width: control.icon.width
            anchors.centerIn: parent
        }
    }
}
