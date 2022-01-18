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

    implicitHeight: implicitContentHeight + topPadding + bottomPadding
    spacing: Maui.Style.space.medium

    property alias slider : _slider
    property alias label : _label

    padding: Maui.Style.space.medium

    signal clicked()
    signal wheel(var wheel)

    background: Rectangle
    {
        readonly property color m_color : Qt.tint(Qt.lighter(control.Kirigami.Theme.textColor), Qt.rgba(control.Kirigami.Theme.backgroundColor.r, control.Kirigami.Theme.backgroundColor.g, control.Kirigami.Theme.backgroundColor.b, 0.9))

        color: Qt.rgba(m_color.r, m_color.g, m_color.b, 0.4)

        radius: 10
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

            Label
            {
                id: _label
                Layout.fillWidth: true
                wrapMode: Text.NoWrap
                elide: Text.ElideMiddle
                font.bold: true
                visible: text && text.length
                color: Kirigami.Theme.textColor
            }

            RowLayout
            {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: control.spacing

                SliderTemplate
                {
                    id: _slider
                    Layout.fillWidth: true
                }

                ToolButton
                {
                    Kirigami.Theme.colorSet: control.Kirigami.Theme.colorSet
                    visible: control.page
                    icon.name: "go-next"
                    onClicked: control.clicked()
                }
            }
        }
    }
}
