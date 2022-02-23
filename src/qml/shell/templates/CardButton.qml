import QtQuick 2.15

import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3

import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.3 as Maui

import QtQuick.Templates 2.15 as T

Maui.BasicToolButton
{
    id: control
    property string tooltipText : text
    Kirigami.Theme.inherit: true
    Kirigami.Theme.colorSet: Kirigami.Theme.Button
    padding: 2
    spacing: 2
    icon.width: 22
    icon.height: 22

    background: Rectangle
    {
        readonly property color m_color : Qt.tint(Qt.lighter(control.Kirigami.Theme.textColor), Qt.rgba(control.Kirigami.Theme.backgroundColor.r, control.Kirigami.Theme.backgroundColor.g, control.Kirigami.Theme.backgroundColor.b, 0.9))

        color: control.hovered ? control.Kirigami.Theme.hoverColor :( control.checked ||  control.containsPress ? control.Kirigami.Theme.highlightColor : Qt.rgba(m_color.r, m_color.g, m_color.b, 0.4))

        radius: 10

        Behavior on color
        {
            ColorAnimation
            {
                easing.type: Easing.InQuad
                duration: Kirigami.Units.longDuration
            }
        }
    }

    ToolTip.text: control.tooltipText
    ToolTip.delay: 1000
    ToolTip.timeout: 5000
    ToolTip.visible: control.hovered && control.tooltipText

    //    contentItem: Maui.GridItemTemplate
    //    {
    //        hovered: control.hovered
    //        iconVisible: true
    //        iconSource: control.icon.name
    //        label1.text: control.text
    //        iconSizeHint: control.icon.width
    //        labelSizeHint: 32
    //    }
}
