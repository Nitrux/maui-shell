/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Copyright (C) 2020 Shawn Rutledge
**
** This file is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 3 as published by the Free Software Foundation
** and appearing in the file LICENSE included in the packaging
** of this file.
**
** This code is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU Lesser General Public License for more details.
**
****************************************************************************/

import QtQuick 2.15
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.3

import QtWayland.Compositor 1.15
import QtGraphicalEffects 1.15

import org.mauikit.controls 1.3 as Maui
import org.kde.kirigami 2.14 as Kirigami
import org.maui.cask 1.0 as Cask
import Zpaces 1.0 as ZP

Cask.StackableItem
{
    id: rootChrome

    readonly property bool intersects : toplevel ? (y+height > _zpaceContainer.height+20 || formFactor !== Cask.Env.Desktop) && activated : false
    property alias shellSurface: surfaceItem.shellSurface

    property bool overviewMode : false

    property ZP.XdgWindow window
    property XdgSurface xdgSurface: window.xdgSurface
    property XdgToplevel toplevel: window.toplevel
    property WaylandSurface surface: xdgSurface.surface
    property WaylandSurface parentSurface: toplevel.parentToplevel.xdgSurface.surface


    readonly property string appId: window.appId
    readonly property string title: window.title

    property bool activated : toplevel.activated
    property bool resizing : toplevel.resizing


    property alias moveItem: surfaceItem.moveItem

    readonly property bool decorationVisible: win.formFactor === Cask.Env.Desktop && toplevel.decorationMode === XdgToplevel.ServerSideDecoration

    property bool moving: surfaceItem.moveItem ? surfaceItem.moveItem.moving : false
    property alias destroyAnimation : destroyAnimationImpl

    property int marginWidth : window.fullscreen ? 0 : (surfaceItem.isPopup ? 1 : 6)
    //    property int titlebarHeight : surfaceItem.isPopup || surfaceItem.isFullscreen ? 0 : 25

    readonly property int titlebarHeight : decorationVisible ? 36 : 0
    property string screenName: ""

    property real resizeAreaWidth: 12

    property rect previousRect

    x: surfaceItem.moveItem.x - surfaceItem.output.geometry.x
    y: surfaceItem.moveItem.y - surfaceItem.output.geometry.y

    height: surfaceItem.height + titlebarHeight
    width: surfaceItem.width

    visible: surfaceItem.valid && surfaceItem.paintEnabled

    onIntersectsChanged:
    {
        if(intersects)
            dock.hide()
        else
            dock.show()
    }

    property rect oldPos : Qt.rect(0, 0, rootChrome.width * 0.6, rootChrome.height*0.6)

    function performActiveWindowAction(type)
    {
        if (type === Maui.CSDButton.Close)
        {
            destroyAnimationImpl.start()

        } else if (type === Maui.CSDButton.Maximize)
        {
            window.maximize()
        }
        else if (type === Maui.CSDButton.Restore)
        {
            window.unmaximize()
        }
        if (type ===  Maui.CSDButton.Minimize) {
            window.minimize()
        }
    }

    Connections
    {
        target: rootChrome.window

        function onSetMinimized()
        {
            rootChrome.visible = false
            window.deactivate()
        }

        function onUnsetMinimized()
        {
            rootChrome.visible = true;
            surfaceItem.forceActiveFocus();
            window.activate()
        }

        function onSetMaximized()
        {
            console.log("SET MAX", toplevel.maximized, oldPos)

            oldPos.x = rootChrome.x
            oldPos.y = rootChrome.y
            oldPos.width = rootChrome.width
            oldPos.height = rootChrome.height

            rootChrome.x = 0
            rootChrome.y = 0


            toplevel.sendMaximized(Qt.size(_zpaceContainer.width, _zpaceContainer.height - titlebarHeight))


            //            window.activate()

        }

        function onUnsetMaximized()
        {
            console.log("SET UNMAX", toplevel.maximized, oldPos)
            if(oldPos.width === -1)
            {
                oldPos = Qt.rect(0, 0, rootChrome.width * 0.6, rootChrome.height*0.6)
            }

            rootChrome.x = oldPos.x
            rootChrome.y = oldPos.y
            toplevel.sendUnmaximized(Qt.size(oldPos.width, oldPos.height))

        }

        function onSetFullScreen()
        {

        }
    }

    Connections
    {
        target: rootChrome.toplevel
        ignoreUnknownSignals: true

        function onActivatedChanged ()
        { // xdg_shell only
            console.log("ACTIVATED CHANGED", toplevel.activated)
            if (target.activated)
            {
                receivedFocusAnimation.start();
            }
        }

        function onMaximizedChanged()
        {
            console.log("REQUESTED", target.maximized)

        }

        function onFullscreenChanged()
        {
            console.log("REQUESTED FULLSCREEN", target.fullscreen)
        }
    }

    Component.onCompleted:
    {
        control.window.chrome = control
        surfaceItem.forceActiveFocus()
    }

    //    Component.onDestruction: intersects = false

    Rectangle
    {
        id: decoration

        //        Kirigami.Theme.inherit: false
        Kirigami.Theme.colorSet: Kirigami.Theme.Window
        anchors.fill: parent
        border.width: 1
        radius: window.maximized ? 0 : Maui.Style.radiusV
        border.color: window.maximized ? "transparent" : (rightEdgeHover.hovered || bottomEdgeHover.hovered) ? "#ffc02020" :"#305070a0"
        color: Kirigami.Theme.backgroundColor
        visible: rootChrome.decorationVisible

        TapHandler
        {
            onTapped: surfaceItem.forceActiveFocus()
        }

        Item
        {
            id:  titleBar
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: Maui.Style.space.medium
            anchors.rightMargin: Maui.Style.space.medium
            height: titlebarHeight
            visible: !surfaceItem.isPopup

            DragHandler
            {
                id: titlebarDrag
                target: rootChrome
                //                enabled: rootChrome.activated
                cursorShape: Qt.ClosedHandCursor
                onActiveChanged:
                {
                    if(!active && toplevel.maximized)
                    {
                        window.unmaximize()
                    }
                }

                property var movingBinding: Binding
                {
                    target: surfaceItem.moveItem
                    property: "moving"
                    value: titlebarDrag.active
                }
            }

            HoverHandler
            {
                cursorShape: Qt.OpenHandCursor
            }

            TapHandler
            {
                acceptedButtons: Qt.LeftButton
                gesturePolicy: TapHandler.DragThreshold
                onTapped: rootChrome.raise()
            }

            TapHandler
            {
                acceptedButtons: Qt.MiddleButton
                gesturePolicy: TapHandler.DragThreshold
                onTapped: rootChrome.lower()
            }

            RowLayout
            {
                anchors.fill: parent

                Loader
                {
                    id: _leftControlsLoader
                    visible: active
                    active: Maui.App.leftWindowControls.length
                    Layout.preferredWidth: active ? implicitWidth : 0
                    sourceComponent: Maui.CSDControls
                    {
                        side: Qt.LeftEdge
                        onButtonClicked: performActiveWindowAction(type)
                    }
                }

                Label
                {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    text: rootChrome.title
                    horizontalAlignment: Qt.AlignHCenter
                    elide: Text.ElideMiddle
                    wrapMode: Text.NoWrap
                    color: Kirigami.Theme.textColor
                }

                Loader
                {
                    id: _rightControlsLoader
                    visible: active
                    active: Maui.App.rightWindowControls.length
                    Layout.preferredWidth: active ? implicitWidth : 0
                    sourceComponent: Maui.CSDControls
                    {
                        side: Qt.RightEdge
                        maximized: rootChrome.toplevel.maximized
                        isActiveWindow: rootChrome.activated
                        onButtonClicked: rootChrome.performActiveWindowAction(type)
                    }
                }
            }
        }


        // TODO write a ResizeHandler for this purpose? otherwise there are up to 8 components for edges and corners
        Item
        {
            enabled: !window.maximized
            focus: false
            id: rightEdgeResizeArea
            x: parent.width - resizeAreaWidth / 2;
            width: resizeAreaWidth; height: parent.height - resizeAreaWidth

            onXChanged:
                if (horzDragHandler.active)
                {
                    var size = toplevel.sizeForResize(horzDragHandler.initialSize,
                                                      Qt.point(horzDragHandler.translation.x,
                                                               horzDragHandler.translation.y),
                                                      Qt.RightEdge);
                    toplevel.sendResizing(size)
                }

            DragHandler {
                id: horzDragHandler
                property size initialSize
                onActiveChanged: if (active) initialSize = Qt.size(rootChrome.width, rootChrome.height)
                yAxis.enabled: false
            }
            HoverHandler {
                id: rightEdgeHover
                cursorShape: Qt.SizeHorCursor // problem: this so far only sets the EGLFS cursor, not WaylandCursorItem
            }
        }

        Item
        {
            enabled: !window.maximized

            focus: false
            id: bottomEdgeResizeArea
            y: parent.height - resizeAreaWidth / 2; height: resizeAreaWidth; width: parent.width - resizeAreaWidth
            onYChanged:
                if (vertDragHandler.active) {
                    var size = toplevel.sizeForResize(vertDragHandler.initialSize,
                                                      Qt.point(vertDragHandler.translation.x, vertDragHandler.translation.y),
                                                      Qt.BottomEdge);
                    toplevel.sendResizing(size)
                }
            DragHandler {
                id: vertDragHandler
                property size initialSize
                onActiveChanged: if (active) initialSize = Qt.size(rootChrome.width, rootChrome.height)
                xAxis.enabled: false
            }
            HoverHandler {
                id: bottomEdgeHover
                cursorShape: Qt.SizeVerCursor
            }
        }

        Item
        {
            enabled: !window.maximized

            focus: false
            id: bottomRightResizeArea
            x: parent.width - resizeAreaWidth / 2; y: parent.height - resizeAreaWidth / 2
            width: resizeAreaWidth; height: parent.height - resizeAreaWidth
            onXChanged: resize()
            onYChanged: resize()
            function resize() {
                if (bottomRightDragHandler.active) {
                    var size = toplevel.sizeForResize(bottomRightDragHandler.initialSize,
                                                      Qt.point(bottomRightDragHandler.translation.x, bottomRightDragHandler.translation.y),
                                                      Qt.BottomEdge | Qt.RightEdge);
                    toplevel.sendResizing(size)
                }
            }
            DragHandler {
                id: bottomRightDragHandler
                property size initialSize
                onActiveChanged: if (active) initialSize = Qt.size(rootChrome.width, rootChrome.height)
            }
            HoverHandler {
                id: bottomRightHover
                cursorShape: Qt.SizeFDiagCursor
            }
        }
        // end of resizing components
    }

    Connections
    {
        target: _zpaceContainer
        ignoreUnknownSignals: true
        enabled: win.formFactor !== Cask.Env.Desktop

        function onWidthChanged()
        {
            if(formFactor !== Cask.Env.Desktop)
            {
                window.maximize()
            }
        }

        function onHeightChanged()
        {
            if(formFactor !== Cask.Env.Desktop)
            {
                window.maximize()
            }
        }
    }

    Connections
    {
        target: win
        ignoreUnknownSignals: true
        function onFormFactorChanged()
        {
            if(win.formFactor === Cask.Env.Desktop)
            {
                rootChrome.shellSurface.toplevel.sendConfigure(Qt.size(previousRect.width, previousRect.height), [3,4])
                rootChrome.x = previousRect.x
                rootChrome.y = previousRect.y
            }else
            {
                previousRect = Qt.rect(rootChrome.x, rootChrome.y, rootChrome.width, rootChrome.height)
                rootChrome.x = output.geometry.x
                rootChrome.y = output.geometry.y
            }
        }
    }


    SequentialAnimation {
        id: destroyAnimationImpl
        ParallelAnimation {
            NumberAnimation { target: scaleTransform; property: "yScale"; to: 2/height; duration: 150 }
            NumberAnimation { target: scaleTransform; property: "xScale"; to: 0.4; duration: 150 }
        }
        NumberAnimation { target: scaleTransform; property: "xScale"; to: 0; duration: 150 }
        ScriptAction { script: { window.close()  } }
    }

    ParallelAnimation {
        id: createAnimationImpl
        NumberAnimation { target: scaleTransform; property: "yScale"; from: 0; to: 1; duration: 150 }
        NumberAnimation { target: scaleTransform; property: "xScale"; from: 0; to: 1; duration: 150 }
    }

    SequentialAnimation {
        id: receivedFocusAnimation
        ParallelAnimation {
            NumberAnimation { target: scaleTransform; property: "yScale"; to: 1.02; duration: 100; easing.type: Easing.OutQuad }
            NumberAnimation { target: scaleTransform; property: "xScale"; to: 1.02; duration: 100; easing.type: Easing.OutQuad }
        }
        ParallelAnimation {
            NumberAnimation { target: scaleTransform; property: "yScale"; to: 1; duration: 100; easing.type: Easing.InOutQuad }
            NumberAnimation { target: scaleTransform; property: "xScale"; to: 1; duration: 100; easing.type: Easing.InOutQuad }
        }

        ScriptAction { script: { surfaceItem.forceActiveFocus()  } }

    }

    transform: [
        Scale {
            id:scaleTransform
            origin.x: rootChrome.width / 2
            origin.y: rootChrome.height / 2
        }
    ]

    //    FastBlur
    //    {
    //        id: fastBlur
    //        anchors.fill: parent
    //        source: _cask
    //        radius: 100
    //        transparentBorder: false
    //        cached: true
    //    }




    ShellSurfaceItem
    {
        id: surfaceItem
        property bool valid: false
        property bool isPopup: false
        property bool isTransient: false

        y: titlebarHeight
        sizeFollowsSurface: false
        opacity: moving || pinch4.activeScale <= 0.5 ? 0.5 : 1.0
        inputEventsEnabled: !rootChrome.overviewMode
        touchEventsEnabled: !pinch3.active && !pinch4.active
        //        paintEnabled: visible

        focusOnClick:  !altDragHandler.active && !rootChrome.overviewMode
        autoCreatePopupItems: true

        onActiveFocusChanged:
        {
            if(activeFocus)
            {
                rootChrome.raise()
                rootChrome.window.activate()
            }else
            {
                rootChrome.window.deactivate()
            }
        }

        DragHandler {
            id: metaDragHandler
            acceptedModifiers: Qt.MetaModifier
            target: surfaceItem.moveItem
            property var movingBinding: Binding {
                target: surfaceItem.moveItem
                property: "moving"
                value: metaDragHandler.active
            }
        }

        DragHandler {
            id: altDragHandler
            acceptedModifiers: Qt.AltModifier
            target: surfaceItem.moveItem
            property var movingBinding: Binding {
                target: surfaceItem.moveItem
                property: "moving"
                value: altDragHandler.active
            }
        }

        Connections
        {
            target: shellSurface
            ignoreUnknownSignals: true



            //            onSetPopup: {
            //                surfaceItem.isPopup = true
            //                decoration.visible = false
            //            }

            //            onSetTransient: {
            //                surfaceItem.isTransient = true
            //            }

            //            onSetFullScreen: {
            //                surfaceItem.isFullscreen = true
            //                rootChrome.x = 0
            //                rootChrome.y = 0
            //            }

            onSetMaximized:
            {
                console.log("EVENT SEND MAXIMIZED CATCHED <<<<<<<<<<")


            }

            onUnsetMaximized:
            {

            }

            onSetMinimized:
            {

            }
        }

        onSurfaceDestroyed: {
            destroyAnimationImpl.start();
        }

        onWidthChanged: {
            valid =  !surface.cursorSurface && surface.size.width > 0 && surface.size.height > 0
        }

        //        onValidChanged: if (valid) {
        //                            if (isFullscreen) {
        //                                toplevel.sendFullscreen(output.geometry)
        //                            } else if (decorationVisible) {
        //                                createAnimationImpl.start()
        //                            }
        //                        }
    }

    layer.enabled: _borders.visible
    layer.effect: OpacityMask
    {
        maskSource: Rectangle
        {
            width: Math.floor(rootChrome.width)
            height: Math.floor(rootChrome.height)
            radius: _borders.radius
        }

        layer.enabled: _borders.visible
        layer.effect :  DropShadow
        {
            transparentBorder: true
            horizontalOffset: 0
            verticalOffset: 0
            color: Qt.rgba(0,0,0,0.2)
        }
    }

    Rectangle
    {
        id: _borders
        anchors.fill: parent
        visible: win.formFactor === Cask.Env.Desktop ? (rootChrome.decorationVisible && !window.maximized) : (rootChrome.height < availableGeometry.height || rootChrome.width < availableGeometry.width || pinch4.active)
        z: surfaceItem.z + 9999999999
        //         anchors.margins: Maui.Style.space.small
        radius: decoration.radius
        color: "transparent"
        border.color: Qt.darker(Kirigami.Theme.backgroundColor, 2.7)
        opacity: 0.8
        border.width: 1
        Rectangle
        {
            anchors.fill: parent
            anchors.margins: 1
            color: "transparent"
            radius: parent.radius - 0.5
            border.width: 1
            border.color: Qt.lighter(Kirigami.Theme.backgroundColor, 2)
            opacity: 0.2
        }
    }

    DragHandler {
        id: pinch3
        enabled: _borders.visible
        objectName: "3-finger pinch"
        minimumPointCount: 3
        maximumPointCount: 3
        grabPermissions: PointerHandler.CanTakeOverFromAnything

    }

    //    NumberAnimation on x{
    //    id: anim
    //    running: pinch4.activeScale <= 0.5
    //    to: 60
    //    duration: 100
    //    onStopped: {
    //    if(anim.to===60) { anim.from=60; anim.to=50; } else { anim.from=50; anim.to=60 }
    //    start()
    //    }
    //    }


    PinchHandler
    {
        id: pinch4
        objectName: "4-finger pinch"
        minimumPointCount: 4
        maximumPointCount: 4
        minimumScale: 0.5
        maximumScale: 2
        minimumRotation: 0
        maximumRotation: 0

        onActiveChanged: if (!active) {
                             // just a silly way of getting a QSize for sendConfigure()

                             if(activeScale <= minimumScale)
                             {
                                 surfaceItem.surface.client.close()
                             }

                             if(width * scale > availableGeometry.width*0.9 && height * scale > availableGeometry.height*0.9)
                             {
                                 window.maximize()
                                 rootChrome.scale = 1
                                 rootChrome.x =0
                                 rootChrome.y = 0

                                 return;
                             }

                             var minWidth = availableGeometry.width/2
                             var minHeight = availableGeometry.height/2
                             if(width*scale <= minWidth && height *scale < minHeight)
                             {

                                 rootChrome.scale = 1


                                 return;
                             }

                             var size = toplevel.sizeForResize(Qt.size(width * scale, height * scale), Qt.point(0, 0), 0);
                             toplevel.sendConfigure(size, [3] /*XdgShellToplevel.ResizingState*/);
                             rootChrome.scale = 1
                             rootChrome.x = pinch4.centroid.scenePosition.x -(size.width/2)
                             rootChrome.y = pinch4.centroid.scenePosition.y-(size.height/2)
                         }
    }

    DragHandler
    {
        enabled: rootChrome.overviewMode
        xAxis.enabled: true
        onActiveChanged:
        {
            if(!active && (target.y * -1) > 100)
                window.close()
            //            else target.y = 0
        }
    }



    //    Rectangle {
    //        z: surfaceItem.z + 9999999999
    //        visible: true
    //        border.color: "white"
    //        color: "black"
    //        radius: 5
    //        anchors.centerIn: parent
    //        width: height * 10
    //        height: moveGeometryText.implicitHeight * 1.5
    //        Text {
    //            id: moveGeometryText
    //            color: "white"
    //            anchors.centerIn: parent
    //            text: Math.round(rootChrome.x) + "," + Math.round(rootChrome.y) + " on " + rootChrome.screenName + "\n" + Math.round(surfaceItem.output.geometry.height) + "," + Math.round(rootChrome.height) + " ," + rootChrome.scale + " / " + pinch4.activeScale
    //        }

    //    }
}
