import QtQuick 2.12
import QtGraphicalEffects 1.0

import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5
import org.kde.kirigami 2.7 as Kirigami
import org.kde.mauikit 1.2 as Maui

Item
{
    id: control

    default property alias content: _layout.data
    implicitHeight: _layout.implicitHeight + _page.header.height + _page.footer.height + _page.padding + Maui.Style.space.big
    width: popWidth
    height: implicitHeight

    property alias title : _page.title
    property alias padding : _page.padding
    property alias headBar: _page.headBar
    property alias footBar: _page.footBar

    Item
    {
        id: _rec
        anchors.fill: parent
        anchors.margins: Maui.Style.space.tiny

        Maui.Page
        {
            id: _page
            showTitle: true
            anchors.fill: parent

            padding:   Maui.Style.space.small
            background: Rectangle
            {
                color: Kirigami.Theme.backgroundColor
                border.color: Qt.tint(Kirigami.Theme.textColor, Qt.rgba(Kirigami.Theme.backgroundColor.r, Kirigami.Theme.backgroundColor.g, Kirigami.Theme.backgroundColor.b, 0.7))
                radius: Maui.Style.radiusV
            }

            Column
            {
                id: _layout
                anchors.fill: parent
            }

            layer.enabled: true
            layer.effect: OpacityMask
            {
                maskSource: Item
                {
                    width: control.width
                    height: control.height

                    Rectangle
                    {
                        anchors.fill: parent
                        radius: Maui.Style.radiusV
                    }
                }
            }
        }
    }

    DropShadow
    {
        transparentBorder: true
        anchors.fill: _rec
        horizontalOffset: 0
        verticalOffset: 0
        radius: 8.0
        samples: 17
        color: Qt.rgba(0,0,0,0.2)
        source: _rec
    }
}
