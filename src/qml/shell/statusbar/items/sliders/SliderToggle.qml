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

    implicitHeight: 32 + topPadding + bottomPadding

    property alias slider : _slider

    padding: Maui.Style.space.medium

    signal clicked()

    background: Rectangle
    {
        readonly property color m_color : Qt.tint(Qt.lighter(control.Kirigami.Theme.textColor), Qt.rgba(control.Kirigami.Theme.backgroundColor.r, control.Kirigami.Theme.backgroundColor.g, control.Kirigami.Theme.backgroundColor.b, 0.9))

        color: Qt.rgba(m_color.r, m_color.g, m_color.b, 0.4)

        radius: 10
    }

    contentItem: RowLayout
    {
        SliderTemplate
        {
            id: _slider
            Layout.fillWidth: true
        }

        ToolButton
        {
            visible: control.page
            icon.name: "go-next"
            onClicked: control.clicked()
        }
    }


}
