import QtQuick 2.13
import QtQuick.Controls 2.13
import QtQuick.Layouts 1.3

import org.mauikit.controls 1.3 as Maui

import org.maui.cask 1.0 as Cask

import "../../../templates"

CardButton
{
    id: control
    implicitHeight: 32 + topPadding + bottomPadding
    implicitWidth: Math.max(_template.layout.implicitWidth, implicitHeight) + leftPadding + rightPadding
    property alias template : _template

    display: ToolButton.IconOnly

    property Component page : null

    Behavior on implicitHeight
    {
        NumberAnimation
        {
            duration: Maui.Style.units.shortDuration
            easing.type: Easing.OutQuad
        }
    }

    contentItem: Maui.ListItemTemplate
    {
        id: _template

        spacing: control.spacing
        labelsVisible: (label1.text.length || label2.text.length) && control.display === ToolButton.TextBesideIcon
        label1.text: control.text
        label1.wrapMode: Text.Wrap
        iconSource: control.icon.name
        isMask: true
        isCurrentItem: control.checked
        iconVisible: true
        iconSizeHint: Maui.Style.iconSize
//        headerSizeHint: control.implicitHeight
    }
}
