import QtQuick 2.15
import QtQuick.Controls 2.15

import org.kde.kirigami 2.7 as Kirigami
import org.mauikit.controls 1.2 as Maui
import org.maui.cask 1.0 as Cask
import Zpaces 1.0 as ZP

import QtGraphicalEffects 1.0

Control
{
    id: control
    property ZP.Zpace zpace

    default property alias content: _content.data
    property int radius: 20
    property bool overviewMode : false
    property alias backgroundImage : _img.source
    readonly property bool rise : _dropArea.containsDrag

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
                anchors.margins:  control.rise ? Maui.Style.space.large * 2 : 0

                fillMode: Image.PreserveAspectCrop
                layer.enabled: control.radius > 0 || control.rise
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

        MouseArea
        {
            enabled: overviewMode
            anchors.fill: parent
            propagateComposedEvents: true
            preventStealing: false
            onClicked:
            {
                _swipeView.closeOverview()

                _swipeView.currentIndex = index
                mouse.accepted= false
            }
        }
    }


    DropArea
    {
        id: _dropArea

        anchors.fill: parent
        onDropped:
        {
            if(drop.urls)
            {
                control.zpace.wallpaper= drop.urls[0]
            }
        }
    }

}
