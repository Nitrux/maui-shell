import QtQuick 2.15
import QtQuick.Window 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.15
import org.kde.kirigami 2.7 as Kirigami
import org.mauikit.controls 1.2 as Maui
import org.maui.cask 1.0 as Cask
import QtGraphicalEffects 1.0

Item
{
    id: control
    default property alias content: _content.data

    readonly property alias avaliableHeight : _content.height
    readonly property alias avaliableWidth : _content.width
    readonly property alias contentY: _content.y
    readonly property alias surface : _content

    property alias background : _rec
    property alias overlayTopPanel : _overlayTopPanel
    property alias overlayBottomPanel : _overlayBottomPanel
    property alias backgroundColor : _rec.color
    property alias backgroundImage: _img.source

    property alias topPanel: _topPanel
    property alias bottomPanel: _bottomPanel

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
        id: _topPanelContainer
        z: _content.z+2
        //            color: "yellow"
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: _topPanel.visible ? _topPanel.implicitHeight : 0
        Cask.Panel
        {
            id: _topPanel
            anchors.fill: parent
        }
    }

    Item
    {
        id: _container
        anchors.top: _topPanelContainer.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom


        Item
        {
            id: _content
            anchors.fill: parent
        }

        //            TapHandler
        //            {
        //                grabPermissions: PointerHandler.CanTakeOverFromAnything
        //                gesturePolicy: TapHandler.ReleaseWithinBounds | TapHandler.WithinBounds
        //                onTapped:
        //                {
        //                    console.log("Dekstop tapped")
        //                    control.desktopPressed()
        //                }
        //            }

        //            MouseArea
        //            {
        //                anchors.fill: parent
        //                onPressed:
        //                {
        //                    control.desktopPressed()
        //                    mouse.accepted = false
        //                }
        //            }


    }

    Item
    {
        id: _overlayTopPanel
        z: _content.z+1
        anchors.fill: parent
    }

    Item
    {
        id: _overlayBottomPanel
        z: _topPanel.z+2
        anchors.fill: parent
    }

    Item
    {
        id: _bottomPanelContainer
        z: _overlayBottomPanel.z+2

        anchors.left: parent.left
        anchors.bottom: _container.bottom
        anchors.right: parent.right
        height: _bottomPanel.visible ? _bottomPanel.implicitHeight : 0

        Cask.Panel
        {
            id: _bottomPanel
            focus: true
            readonly property int hidden : y === height
            //            y: _dockHoverHandler.hovered ? 0 : height


            height: parent.height
            width: parent.width

            Behavior on y
            {
                NumberAnimation
                {
                    duration: Kirigami.Units.longDuration*3
                    easing.type: Easing.OutInQuad
                }
            }

            function toggle()
            {
                if(_bottomPanel.hidden)
                {
                    show()
                }else
                {
                    hide()
                }
            }

            function hide()
            {
                _bottomPanel.y = height
            }

            function show()
            {
                _bottomPanel.y = 0
            }
        }

        DragHandler
        {
            id: handler
            //            dragThreshold: 100
            //            enabled: _bottomPanel.hidden
            target: _bottomPanel
            yAxis.minimum: 0
            yAxis.maximum: _bottomPanel.height
            xAxis.enabled : false
            grabPermissions: PointerHandler.CanTakeOverFromAnything | PointerHandler.ApprovesTakeOverByHandlersOfSameType
            onActiveChanged:
            {
                if(!active && handler.centroid.scenePressPosition.y -handler.centroid.scenePosition.y > 60)
                {
                    _bottomPanel.show()
                }else
                {
                    _bottomPanel.hide()
                }
            }
        }

        Timer
        {
            id: _dockRevealTimer
            interval: 2500
            //            triggeredOnStart: true
            onTriggered:
            {
                if(_bottomPanel.hidden)
                {
                    _bottomPanel.show()
                }else
                {
                    _bottomPanel.hide()
                }
            }
        }

        HoverHandler
        {
            id: _dockHoverHandler
            enabled: !handler.active
            acceptedPointerTypes: PointerDevice.GenericPointer
            onHoveredChanged:
            {

                _dockRevealTimer.restart()

            }
        }
    }



}
