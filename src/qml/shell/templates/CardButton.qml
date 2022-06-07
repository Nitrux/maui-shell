import QtQuick 2.15

import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3

import org.mauikit.controls 1.3 as Maui

import QtQuick.Templates 2.15 as T

Maui.BasicToolButton
{
    id: control
    property string tooltipText : text

    padding: Maui.Style.space.small
    spacing: padding
    icon.width: 22
    icon.height: 22

    background: Rectangle
    {
        color: control.enabled ? (control.hovered ? Maui.Theme.hoverColor :( control.checked ||  control.containsPress ? Maui.Theme.highlightColor : Maui.Theme.backgroundColor)) :"transparent"

        border.color: control.enabled ? "transparent" : (control.checked ||  control.containsPress ? control.Maui.Theme.highlightColor : m_color)

        radius: Maui.Style.radiusV
        opacity: control.checked? 1 : 0.7

        Behavior on color
        {
            ColorAnimation
            {
                easing.type: Easing.InQuad
                duration: Maui.Style.units.shortDuration
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
