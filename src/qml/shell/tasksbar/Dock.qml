import QtQuick
import QtQml
import QtQuick.Controls
import QtQuick.Layouts

import org.mauikit.controls as Maui
import org.maui.cask as Cask

import QtQuick.Effects

Item
{
    id: control

    implicitHeight: 64+ topPadding+ bottomPadding

    property int padding: Maui.Style.space.medium
    property int topPadding: padding
    property int bottomPadding: padding

    property bool hidden : _taskbar.y === height
    property bool autohide: false

    property alias launcher : _launcher

    Cask.AppsDB
    {
        id: _appsDB
    }

    Rectangle
    {
        id: overlay
        visible: _launcher.visible
        parent: _cask.overlay
        //        opacity: formFactor !== Cask.MauiMan.Desktop ? 0.95 : 0.7

        anchors.fill: parent
//        color: _launcher.Maui.Theme.backgroundColor

        Loader
        {
            active: Maui.Style.enableEffects
            anchors.fill: parent

            sourceComponent: MultiEffect
            {
                // opacity: 0.2
                blurEnabled: true
                blurMax: 64
                saturation: -0.5
                blur: 1.0
                autoPaddingEnabled: true
                source: _cask.container


                Rectangle
                {
                    anchors.fill: parent
                    color: overlay.color
                    opacity: 0.8
                }
            }
        }

        MouseArea
        {
            anchors.fill: parent
            propagateComposedEvents: false
            preventStealing: true

            onClicked:
            {
                _launcher.close()
                mouse.accepted = true
            }
        }
    }

    data: WheelHandler
    {
        //        orientation: Qt.Vertical
        target: null
        enabled: true
        grabPermissions: PointerHandler.CanTakeOverFromAnything

        //        property: "y"
        //        rotationScale: 15
        //        acceptedDevices: PointerDevice.Mouse | PointerDevice.TouchPad
        onActiveChanged:
        {
            console.log("WHEEL HANMDLER ACTIVE", rotation)

            if(!active)
            {
                if(rotation > 15)
                {
                    launcher.open()
                }
                else if(rotation < -60)
                {
                    if(launcher.opened)
                    {
                        return
                    }

                    launcher.close()
                }
                rotation = 0
            }
        }
    }

    Launcher
    {
        id: _launcher
        visible: handler2.active || opened
        width: Math.min(1200, parent.width)
        height: fullscreenLauncher ? _cask.avaliableHeight- (control.height) : Math.min(_cask.avaliableHeight - (control.height) , 800)

        Binding on y
        {
            value: handler2.active ? handler2.centroid.position.y : (_launcher.opened ? _launcher.finalYPos : control.height)
            restoreMode: Binding.RestoreBindingOrValue
        }

        x: handler2.active && win.formFactor === Cask.MauiMan.Desktop && !_launcher.opened ? (handler2.centroid.scenePressPosition.x - (width/2)) :  Math.round((control.width/2 ) - (_launcher.width/2));

    }

    Item
    {
        id: _container
        anchors.fill: parent
        anchors.margins: control.padding

        TaskBar
        {
            id: _taskbar
            width: Math.min(implicitWidth, parent.width)
            height: parent.height
            x: parent.width/2 - width/2

            Behavior on y
            {
                NumberAnimation
                {
                    duration: 540
                    easing.type: Easing.OutBack
                }
            }

            DragHandler
            {
                id: handler
                //            dragThreshold: 100
                //            target: _bottomPanelContainer
                margin: control.height/2
                yAxis.minimum: 0
                yAxis.maximum: control.height
                xAxis.enabled : screen.orientation === Qt.PortraitOrientation ||  screen.orientation === Qt.InvertedPortraitOrientation || screen.orientation === Qt.PrimaryOrientation
                yAxis.enabled : screen.orientation === Qt.PortraitOrientation ||  screen.orientation === Qt.InvertedPortraitOrientation || screen.orientation === Qt.PrimaryOrientation

                //            grabPermissions: PointerHandler.CanTakeOverFromAnything | PointerHandler.ApprovesTakeOverByHandlersOfSameType
                onActiveChanged:
                {
                    if(!active && handler.centroid.scenePressPosition.y -handler.centroid.scenePosition.y > 60)
                    {
                        control.show()
                    }else
                    {
                        control.hide()
                    }
                }
            }

            DragHandler
            {
                id: handler2
                target: null

                xAxis.enabled : screen.orientation === Qt.PortraitOrientation ||  screen.orientation === Qt.InvertedPortraitOrientation || screen.orientation === Qt.PrimaryOrientation
                yAxis.enabled: screen.orientation === Qt.LandscapeOrientation ||  screen.orientation === Qt.InvertedLandscapeOrientation || screen.orientation === Qt.PrimaryOrientation

                dragThreshold: control.height+10
                enabled: !_launcher.opened

                grabPermissions: PointerHandler.CanTakeOverFromAnything

                onActiveChanged:
                {
                    const scenePressPos = screen.orientation === Qt.PortraitOrientation ? handler2.centroid.scenePressPosition.x : handler2.centroid.scenePressPosition.y
                    const scenePos = screen.orientation === Qt.PortraitOrientation ? handler2.centroid.scenePosition.x : handler2.centroid.scenePosition.y

                    const condition =  Math.abs(scenePressPos -scenePos)  > 200

                    if(!active && condition)
                    {
                        _launcher.open()
                    }else
                    {
                        _launcher.close()
                    }
                }
            }

            Maui.ShadowedRectangle
            {
                visible: control.hidden
                color: Maui.Theme.backgroundColor
                opacity: 0.7
                width: parent.width
                y: 0-(height)
                height: 20

                corners
                {
                    topLeftRadius: _taskbar.radius
                    topRightRadius: _taskbar.radius
                }

                HoverHandler
                {
                    id: _dockHoverHandler
                    enabled: !handler.active && control.hidden
                    acceptedPointerTypes: PointerDevice.GenericPointer
                    onHoveredChanged:
                    {
                        _dockRevealTimer.restart()
                    }
                }
            }
        }
    }


    Timer
    {
        id: _dockRevealTimer
        interval: 800

        //            triggeredOnStart: true
        onTriggered:
        {
            if(control.hidden && _dockHoverHandler.hovered)
            {
                control.show()
            }
        }
    }


    function toggle()
    {
        if(control.hidden)
        {
            show()
        }else
        {
            hide()
        }
    }

    function hide()
    {
        _taskbar.y = height
    }

    function show()
    {
        _taskbar.y = 0
        //        _taskbar.forceActiveFocus()
    }
}
