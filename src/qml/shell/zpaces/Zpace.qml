import QtQuick 2.15
import QtQuick.Controls 2.15

import org.kde.kirigami 2.7 as Kirigami
import org.mauikit.controls 1.2 as Maui
import org.maui.cask 1.0 as Cask

import QtGraphicalEffects 1.0

Control
{
    id: control

    default property alias content: _content.data
property int radius: 20
    property bool overviewMode : false
property alias backgroundImage : _img.source

    Behavior on radius
    {
        NumberAnimation
        {
            duration: Kirigami.Units.longDuration
            easing.type: Easing.OutInQuad
        }
    }




    background: Item
    {
        id: _bg

        Item
        {
            id : _bgContainer
            anchors.fill: parent

        Image
        {
            id: _img
            anchors.fill: parent
            fillMode: Image.PreserveAspectCrop
            layer.enabled: control.radius > 0
            layer.effect: OpacityMask
            {
                maskSource: Item
                {
                    width: _img.width
                    height: _img.height

                    Rectangle
                    {
                        anchors.fill: parent
                        radius: control.radius
                    }
                }
            }

//            Rectangle
//            {
//                visible:  control.overviewMode
//                anchors.fill: parent
//                //         anchors.margins: Maui.Style.space.small
//                radius: control.radius
//                color: "transparent"
//                border.color: Qt.darker(Kirigami.Theme.backgroundColor, 2.7)
//                opacity: 0.5


//                Rectangle
//                {
//                    anchors.fill: parent
//                    anchors.margins: 1
//                    color: "transparent"
//                    radius: parent.radius - 0.5
//                    border.color: Qt.lighter(Kirigami.Theme.backgroundColor, 2)
//                    opacity: 0.7
//                }
//            }
        }
    }

        DropShadow
        {
//            visible: control.rise
            transparentBorder: true
            anchors.fill: _bgContainer
            horizontalOffset: 0
            verticalOffset: 0
            radius: 8.0
            samples: 17
            color: Qt.rgba(0,0,0,0.9)
            source: _bgContainer
        }


    }

    contentItem : Item
    {
        id: _container

        Item
        {
            id: _content
            anchors.fill: parent
        }

        TapHandler
        {
            target: _content
            onTapped:
            {
                _swipeView.currentIndex = index
                _swipeView.closeOverview()
            }
        }
    }


}
