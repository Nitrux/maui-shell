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
    implicitWidth: 500

    width: currentCard === index ? Math.min(control.parent.width, implicitWidth) :  control.parent.width
    height: visible ? implicitHeight : 0

    x:
    {
        if(currentCard === index)
        {
            switch(alignment)
            {
            case Qt.AlignLeft: return  0;
            case Qt.AlignCenter: return  (control.parent.width / 2) - (control.width / 2);
            case Qt.AlignRight: return  (control.parent.width) - (control.width);

            }
        }else return 0
    }

    property alias title : _page.title
    property alias padding : _page.padding
    property alias headBar: _page.headBar
    property alias footBar: _page.footBar
    property int alignment: Qt.AlignRight

    property int index : -1

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

        Rectangle
        {
            anchors.fill: parent
            //         anchors.margins: Maui.Style.space.small
            radius: Maui.Style.radiusV
            color: "transparent"
            border.color: Qt.darker(Kirigami.Theme.backgroundColor, 2.7)
            opacity: 0.5


            Rectangle
            {
                anchors.fill: parent
                anchors.margins: 1
                color: "transparent"
                radius: parent.radius - 0.5
                border.color: Qt.lighter(Kirigami.Theme.backgroundColor, 2)
                opacity: 0.7
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