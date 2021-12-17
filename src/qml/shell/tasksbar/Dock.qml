import QtQuick 2.15
import QtQml 2.14
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3

import org.kde.kirigami 2.7 as Kirigami
import org.mauikit.controls 1.2 as Maui
import org.maui.cask 1.0 as Cask

Item
{
//    color: "purple"
    id: control
    implicitHeight: _bottomPanelContainer.implicitHeight
    property bool hidden : _container.y === height
    property bool autohide: false
    property alias launcher : _launcher

    Cask.ProcessLauncher
    {
        id: processlauncher
    }

    Rectangle
    {
        id: overlay
        visible: _launcher.visible
        parent: _cask.overlay
        //        z: control.z+2
        opacity: Math.min(formFactor !== Cask.Env.Desktop ? 0.95 : 0.7, _launcher.opacity)

        anchors.fill: parent
        color: _launcher.Kirigami.Theme.backgroundColor

        Behavior on opacity
        {
            NumberAnimation
            {
                duration: Kirigami.Units.longDuration
                easing.type: Easing.OutInQuad
            }
        }

        MouseArea
        {
            anchors.fill: parent
            propagateComposedEvents: win.formFactor === Cask.Env.Desktop
            preventStealing: win.formFactor !== Cask.Env.Desktop
            onPressed:
            {
                _launcher.close()
                mouse.accepted = win.formFactor !== Cask.Env.Desktop
            }
        }
    }

    Launcher
    {
        id: _launcher
        visible: handler2.active || opened
        width: Math.min(800, parent.width)
        height: win.formFactor !== Cask.Env.Desktop ? _cask.avaliableHeight - control.height : Math.min(_cask.avaliableHeight - control.height, 800)

        Binding on y
        {
            value: (_launcher.opened ? _launcher.finalYPos : control.height)
            restoreMode: Binding.RestoreBindingOrValue
        }

        x: handler2.active && win.formFactor === Cask.Env.Desktop && !_launcher.opened ? (handler2.centroid.scenePressPosition.x - (width/2)) :  Math.round((control.width/2 ) - (_launcher.width/2));

    }

    Item
    {
        id: _container
        width: parent.width
        height: parent.height

        RowLayout
        {
            id: _bottomPanelContainer
            width: parent.width
            height: parent.height

            TaskBar
            {
                id: _taskbar

                DragHandler
                {
                    id: handler2
                    target: _launcher
                    xAxis.enabled : false
                    dragThreshold: 64
                    enabled: !_launcher.opened
                    yAxis.minimum: _launcher.finalYPos - 10
                    yAxis.maximum:  0
                    grabPermissions: PointerHandler.CanTakeOverFromAnything

                    onActiveChanged:
                    {
                        const condition = Math.abs(handler2.centroid.scenePressPosition.y -handler2.centroid.scenePosition.y) > 60
                        if(!active && condition)
                        {
                            _launcher.open()
                        }else
                        {
                            _launcher.close()
                        }
                    }
                }
            }
        }

        Behavior on y
        {
            NumberAnimation
            {
                duration: 140
                easing.type: Easing.OutBounce
            }
        }

        DragHandler
        {
            id: handler
            //            dragThreshold: 100
            //            target: _bottomPanelContainer
            margin: 20
            yAxis.minimum: 0
            yAxis.maximum: control.height
            xAxis.enabled : false
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

    Timer
    {
        id: _dockRevealTimer
        interval: 800

        //            triggeredOnStart: true
        onTriggered:
        {
            if(control.hidden)
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
        _container.y = height
    }

    function show()
    {
        _container.y = 0
        _taskbar.forceActiveFocus()
    }

    function launchExec(exec)
    {
        processlauncher.launch(exec)
    }
}
