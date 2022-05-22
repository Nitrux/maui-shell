import QtQuick 2.15
import QtQuick.Controls 2.15

import QtQuick.Layouts 1.3

import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.3 as Maui

import org.maui.cask 1.0 as Cask
import QtQuick.Templates 2.15 as T

T.Pane
{
    id: control
    property Component page : null
    property Item bar

    implicitHeight: implicitContentHeight + topPadding + bottomPadding
    spacing: Maui.Style.space.medium

    property alias label : _label
    property alias label2 : _label2
    property alias icon : _icon

    padding: Maui.Style.space.medium

    signal clicked()
    signal wheel(var wheel)

    background: Rectangle
    {
        readonly property color m_color : Qt.tint(Qt.lighter(control.Kirigami.Theme.textColor), Qt.rgba(control.Kirigami.Theme.backgroundColor.r, control.Kirigami.Theme.backgroundColor.g, control.Kirigami.Theme.backgroundColor.b, 0.9))

        color: m_color
        opacity: 0.7

        radius: 8

        Behavior on color
        {
            ColorAnimation
            {
                easing.type: Easing.InQuad
                duration: Kirigami.Units.shortDuration
            }
        }
    }

    property Item button : ToolButton
    {
        Kirigami.Theme.colorSet: control.Kirigami.Theme.colorSet
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

                Kirigami.Icon
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
                    color: Kirigami.Theme.textColor
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
                    color: Kirigami.Theme.textColor
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
