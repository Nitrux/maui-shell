import QtQuick

import QtQuick.Controls
import QtQuick.Layouts

import org.mauikit.controls as Maui

ToolButton
{
    id: control
    property string tooltipText : text

    background: Rectangle
    {
        color: control.enabled ? (control.hovered ? Maui.Theme.hoverColor :( control.checked ||  control.containsPress ? Maui.Theme.highlightColor : Maui.Theme.backgroundColor)) : "transparent"

        radius: Maui.Style.radiusV

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
}
