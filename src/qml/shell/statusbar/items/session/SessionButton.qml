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

    implicitWidth: 48
    implicitHeight: 64

    icon.width: 22
    icon.height: 22

    background: Rectangle
    {
        readonly property color m_color : Qt.tint(Qt.lighter(control.Kirigami.Theme.textColor), Qt.rgba(control.Kirigami.Theme.backgroundColor.r, control.Kirigami.Theme.backgroundColor.g, control.Kirigami.Theme.backgroundColor.b, 0.9))

        color: control.hovered ? control.Kirigami.Theme.hoverColor :( control.checked ||  control.containsPress ? control.Kirigami.Theme.highlightColor : Qt.rgba(m_color.r, m_color.g, m_color.b, 0.4))

        radius: 10
    }

    contentItem: Maui.GridItemTemplate
    {
        hovered: control.hovered
        iconVisible: true
        iconSource: control.icon.name
        label1.text: control.text
        iconSizeHint: control.icon.width
    }
}
