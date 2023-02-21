import QtQuick 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5

import org.mauikit.controls 1.3 as Maui

import org.maui.cask 1.0 as Cask
import "../templates"
import QtQuick.Templates 2.15 as T

T.AbstractButton
{
    id: control
    Layout.fillHeight: true
    implicitHeight: 48
    implicitWidth: 48

    padding: 0

    icon.height: 22
    icon.width: 22

    property bool flat: Cask.MauiMan.formFactor.preferredMode === Cask.MauiMan.Desktop

    property bool colorize: false

    background: Rectangle
    {
        color: control.colorize ?  _imgColors.highlight  : control.pressed || control.down || control.checked ? Maui.Theme.highlightColor : (control.highlighted || control.hovered ? Maui.Theme.hoverColor : (control.flat ? "transparent" : Maui.Theme.alternateBackgroundColor) )

        opacity: control.checked && control.colorize ? 0.7: 1
        radius: 8

        Maui.ImageColors
        {
            id: _imgColors
            source: control.icon.name
        }

        Behavior on color
        {
            enabled: !control.flat
            ColorAnimation
            {
                easing.type: Easing.InQuad
                duration: Maui.Style.units.shortDuration
            }
        }
    }

    contentItem: Item
    {
        Maui.Icon
        {
            source: control.icon.name
            color: control.checked || control.down ? Maui.Theme.highlightedTextColor : Maui.Theme.textColor
            height: control.icon.height
            width: control.icon.width
            anchors.centerIn: parent
        }
    }
}
