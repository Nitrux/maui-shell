import QtQuick 2.15
import QtQuick.Controls 2.15

import org.kde.kirigami 2.7 as Kirigami
import org.mauikit.controls 1.2 as Maui
import org.maui.cask 1.0 as Cask

import QtGraphicalEffects 1.0

Item
{
    id: control
    default property alias content: _content.data
    property alias overlayContent : _overlayContent.data

    readonly property int avaliableHeight : _content.height-_topPanelContainer.height
    readonly property int avaliableWidth : _content.width

    readonly property rect availableGeometry : Qt.rect(0, _topPanelContainer.height, avaliableWidth, avaliableHeight)

    readonly property alias contentY: _content.y
    readonly property alias surface : _content

    property alias background : _rec
    property alias overlayTopPanel : _overlayTopPanel
    property alias overlay : _overlay
    property alias underneathContent : _underneathContent.data
    property alias backgroundColor : _rec.color
    property alias backgroundImage: _img.source

    property alias topPanel: _topPanel

    readonly property bool rise : _dropArea.containsDrag

    Rectangle
    {
        id: _rec
        anchors.fill: parent
        color: "#333"

        Item
        {
            id: _bg
            anchors.fill: parent
            anchors.margins:  control.rise ? Maui.Style.space.large * 2 : 0
            Image
            {
                id: _img
                anchors.fill: parent
                fillMode: Image.PreserveAspectCrop
                layer.enabled: control.rise
                layer.effect: OpacityMask
                {
                    maskSource: Item
                    {
                        width: _img.width
                        height: _img.height

                        Rectangle
                        {
                            anchors.fill: parent
                            radius: Maui.Style.radiusV
                        }
                    }
                }

                Rectangle
                {
                    visible:  control.rise
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

        }

        DropShadow
        {
            visible: control.rise
            transparentBorder: true
            anchors.fill: _bg
            horizontalOffset: 0
            verticalOffset: 0
            radius: 8.0
            samples: 17
            color: Qt.rgba(0,0,0,0.9)
            source: _bg
        }

        DropArea
        {
            id: _dropArea

            anchors.fill: parent
            onDropped:
            {
                if(drop.urls)
                {
                    _img.source = drop.urls[0]
                }
            }
        }
    }

    Item
    {
        id: _underneathContent
        anchors.fill: parent
    }

    Item
    {
        id: _content
        anchors.fill: parent
    }

    Item
    {
        id: _overlay
        anchors.fill: parent
    }

    Item
    {
        id: _overlayContent
        anchors.fill: parent
    }

    Item
    {
        id: _overlayTopPanel
        //        z: _content.z+1
        anchors.fill: parent
    }

    Item
    {
        id: _topPanelContainer
        //            color: "yellow"
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: _topPanel.visible ? _topPanel.implicitHeight : 0
        //        color : "purple"

        Cask.Panel
        {
            id: _topPanel
            anchors.fill: parent
        }
    }

    Component.onDestruction:
    {
        console.log("DESTROY DASHBOARD")
    }


}
