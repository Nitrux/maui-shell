import QtQuick 2.15
import QtQuick.Controls 2.15

import QtQuick.Layouts 1.3

import org.mauikit.controls 1.3 as Maui

import org.maui.cask 1.0 as Cask
import QtQuick.Templates 2.15 as T

T.Pane
{
    id: control
    property Component page : null
    property Item bar

    Maui.Theme.colorSet: Maui.Theme.Button
    Maui.Theme.inherit: false

    implicitHeight: implicitContentHeight + topPadding + bottomPadding

     padding: Maui.Style.defaultPadding
    spacing: Maui.Style.space.small

    property alias label : _label
    property alias label2 : _label2
    property alias icon : _icon

    signal clicked()
    signal wheel(var wheel)

    background: Rectangle
    {
        color: Maui.Theme.backgroundColor

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

    property Item button : ToolButton
    {
        Maui.Theme.colorSet: control.Maui.Theme.colorSet
        visible: control.page
        icon.name: "go-next"
        onClicked: control.clicked()
    }

    contentItem: MouseArea
    {
        implicitHeight: _layout.implicitHeight
        onWheel: control.wheel(wheel)

        ColumnLayout
        {
            id: _layout
            anchors.fill: parent
            spacing: control.spacing

            RowLayout
            {
                visible: control.label.text.length || control.label2.text.length
                Layout.fillWidth: true
                spacing: control.spacing

                Maui.Icon
                {
                    id: _icon
                    visible: control.icon.source ? String(control.icon.source).length > 0 : false

                    implicitHeight: visible ? 16 : 0
                    implicitWidth: visible ? 16 : 0
                }

                Label
                {
                    id: _label
                    Layout.fillWidth: true
                    wrapMode: Text.NoWrap
                    elide: Text.ElideMiddle
                    font.bold: true
                    visible: text && text.length
                    color: Maui.Theme.textColor
                    horizontalAlignment: Qt.AlignLeft
                    Layout.alignment: Qt.AlignLeft
                }

                Label
                {
                    id: _label2
                    wrapMode: Text.NoWrap
                    elide: Text.ElideNone
                    font.bold: false
                    visible: text && text.length
                    color: Maui.Theme.textColor
                    opacity: 0.7
                    horizontalAlignment: Qt.AlignRight
                    Layout.alignment: Qt.AlignRight
                }
            }

            RowLayout
            {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: control.spacing

                children: [control.bar, control.button]
            }
        }
    }
}
