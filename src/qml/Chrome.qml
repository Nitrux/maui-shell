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
import QtQuick.Window 2.15
import QtQuick.Controls 2.14
import QtWayland.Compositor 1.15
import QtGraphicalEffects 1.15
import org.mauikit.controls 1.3 as Maui
import org.kde.kirigami 2.8 as Kirigami
import QtQuick.Layouts 1.3
import org.maui.cask 1.0 as Cask

Cask.StackableItem
{
    id: rootChrome

    property bool intersects : y+height > availableGeometry.height && surfaceItem.activeFocus
    property alias shellSurface: surfaceItem.shellSurface
    property var topLevel : shellSurface.toplevel
    property alias moveItem: surfaceItem.moveItem
    property bool decorationVisible: win.formFactor === Cask.Env.Desktop && surfaceItem.shellSurface.toplevel.decorationMode === XdgToplevel.ServerSideDecoration
    property bool moving: surfaceItem.moveItem ? surfaceItem.moveItem.moving : false
    property alias destroyAnimation : destroyAnimationImpl

    property int marginWidth : surfaceItem.isFullscreen ? 0 : (surfaceItem.isPopup ? 1 : 6)
    //    property int titlebarHeight : surfaceItem.isPopup || surfaceItem.isFullscreen ? 0 : 25
    property int titlebarHeight : decorationVisible ? 36 : 0
    property string screenName: ""

    property real resizeAreaWidth: 12

    property rect previousRect: Qt.rect(0,0,0,0)

    x: surfaceItem.moveItem.x
    y: surfaceItem.moveItem.y

    height: surfaceItem.height + titlebarHeight
    width: surfaceItem.width
    visible: surfaceItem.valid && surfaceItem.paintEnabled

    onIntersectsChanged:
    {
            if(intersects)
            _cask.bottomPanel.hide()
            else
                _cask.bottomPanel.show()
    }

    property rect oldPos

    function performActiveWindowAction(type)
    {
        if (type === Maui.CSDButton.Close)
        {
            surfaceItem.surface.client.close()
        } else if (type === Maui.CSDButton.Maximize)
        {

            if(surfaceItem.isMaximized)
            {
                rootChrome.x = oldPos.x
                rootChrome.y = oldPos.y
                 surfaceItem.shellSurface.toplevel.sendUnmaximized(Qt.size(oldPos.width, oldPos.height))
                surfaceItem.isMaximized = false

            }else
            {


            oldPos.x = rootChrome.x
            oldPos.y = rootChrome.y
            oldPos.width = rootChrome.width
            oldPos.height = rootChrome.height

            rootChrome.x = 0
            rootChrome.y = 0

            surfaceItem.shellSurface.toplevel.sendMaximized(Qt.size(desktop.availableGeometry.width, desktop.availableGeometry.height - titleBar.height))

                surfaceItem.isMaximized = true

            }

        } if (type ===  Maui.CSDButton.Minimize) {
            rootChrome.visible = false
        }
    }

    Rectangle
    {
        id: decoration

        Kirigami.Theme.inherit: false
        Kirigami.Theme.colorSet: Kirigami.Theme.Window
        anchors.fill: parent
        border.width: 1
        radius: Maui.Style.radiusV
        border.color: (rightEdgeHover.hovered || bottomEdgeHover.hovered) ? "#ffc02020" :"#305070a0"
        color: Kirigami.Theme.backgroundColor
        visible: decorationVisible

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
            height: titlebarHeight - marginWidth
            visible: !surfaceItem.isPopup

            DragHandler {
                id: titlebarDrag
                target: rootChrome
                cursorShape: Qt.ClosedHandCursor
                property var movingBinding: Binding {
                    target: surfaceItem.moveItem
                    property: "moving"
                    value: titlebarDrag.active
                }
            }

            HoverHandler {
                cursorShape: Qt.OpenHandCursor
            }

            TapHandler {
                acceptedButtons: Qt.LeftButton
                gesturePolicy: TapHandler.DragThreshold
                onTapped: rootChrome.raise()
            }

            TapHandler {
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
                    text: surfaceItem.shellSurface.toplevel.title !== undefined ? surfaceItem.shellSurface.toplevel.title : ""
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
                        onButtonClicked: rootChrome.performActiveWindowAction(type)
                    }
                }
            }
        }


        // TODO write a ResizeHandler for this purpose? otherwise there are up to 8 components for edges and corners
        Item {
            id: rightEdgeResizeArea
            x: parent.width - resizeAreaWidth / 2; width: resizeAreaWidth; height: parent.height - resizeAreaWidth
            onXChanged:
                if (horzDragHandler.active) {
                    var size = topLevel.sizeForResize(horzDragHandler.initialSize,
                                                      Qt.point(horzDragHandler.translation.x, horzDragHandler.translation.y),
                                                      Qt.RightEdge);
                    topLevel.sendConfigure(size, [3] /*XdgShellToplevel.ResizingState*/ )
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
        Item {
            id: bottomEdgeResizeArea
            y: parent.height - resizeAreaWidth / 2; height: resizeAreaWidth; width: parent.width - resizeAreaWidth
            onYChanged:
                if (vertDragHandler.active) {
                    var size = topLevel.sizeForResize(vertDragHandler.initialSize,
                                                      Qt.point(vertDragHandler.translation.x, vertDragHandler.translation.y),
                                                      Qt.BottomEdge);
                    topLevel.sendConfigure(size, [3] /*XdgShellToplevel.ResizingState*/ )
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
        Item {
            id: bottomRightResizeArea
            x: parent.width - resizeAreaWidth / 2; y: parent.height - resizeAreaWidth / 2
            width: resizeAreaWidth; height: parent.height - resizeAreaWidth
            onXChanged: resize()
            onYChanged: resize()
            function resize() {
                if (bottomRightDragHandler.active) {
                    var size = topLevel.sizeForResize(bottomRightDragHandler.initialSize,
                                                      Qt.point(bottomRightDragHandler.translation.x, bottomRightDragHandler.translation.y),
                                                      Qt.BottomEdge | Qt.RightEdge);
                    topLevel.sendConfigure(size, [3] /*XdgShellToplevel.ResizingState*/ )
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
        target: output.surfaceArea
        ignoreUnknownSignals: true
        enabled: win.formFactor !== Cask.Env.Desktop
        //                            onHeightChanged:  _chromeDelegate.shellSurface.toplevel.sendConfigure(Qt.size(desktop.availableGeometry.width, surfaceArea.height), [0])
        function onWidthChanged()
        {
            resizeSurface(rootChrome.shellSurface)
        }

        function onHeightChanged()
        {
            resizeSurface(rootChrome.shellSurface)
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
                rootChrome.shellSurface.toplevel.sendConfigure(Qt.size(previousRect.width, previousRect.height), [XdgToplevel.ResizingState])
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
        ScriptAction { script: { _listSurfaces.remove(index) } }
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
        property bool isFullscreen: true
        property bool isMaximized: false

        y: titlebarHeight
        sizeFollowsSurface: true
        opacity: moving ? 0.5 : 1.0
        inputEventsEnabled: !pinch3.active && !metaDragHandler.active && !altDragHandler.active
        touchEventsEnabled: false
//paintEnabled: false
        focusOnClick: true
        autoCreatePopupItems: true

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

            onActivatedChanged: { // xdg_shell only
                if (shellSurface.activated)
                    receivedFocusAnimation.start();
            }

            onSetPopup: {
                surfaceItem.isPopup = true
                decoration.visible = false
            }

            onSetTransient: {
                surfaceItem.isTransient = true
            }

            onSetFullScreen: {
                surfaceItem.isFullscreen = true
                rootChrome.x = 0
                rootChrome.y = 0
            }

            onSetMaximized:
            {
                console.log("EVENT SEND MAXIMIZED CATCHED <<<<<<<<<<")


            }

            onUnsetMaximized:
            {

            }

            onSetMinimized:
            {
                surfaceItem.valid = false
                rootChrome.visible = false
            }
        }

        onSurfaceDestroyed: {
            destroyAnimationImpl.start();
        }

        onWidthChanged: {
            valid =  !surface.cursorSurface && surface.size.width > 0 && surface.size.height > 0
        }

        onValidChanged: if (valid) {
                            if (isFullscreen) {
                                topLevel.sendFullscreen(output.geometry)
                            } else if (decorationVisible) {
                                createAnimationImpl.start()
                            }
                        }
    }

    layer.enabled: rootChrome.decorationVisible
    layer.effect: OpacityMask
    {
        maskSource: Item
        {
            width: rootChrome.width
            height: rootChrome.height

            Rectangle
            {
                anchors.fill: parent
                radius: Maui.App.controls.borderRadius
            }
        }
    }

    Rectangle
    {
        anchors.fill: parent
        visible: decoration.visible
        z: surfaceItem.z + 9999999999
        //         anchors.margins: Maui.Style.space.small
        radius: Maui.App.controls.borderRadius
        color: "transparent"
        border.color: Qt.darker(Kirigami.Theme.backgroundColor, 2.7)
        opacity: 0.8

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

    PinchHandler {
        id: pinch3
        objectName: "3-finger pinch"
        minimumPointCount: 3
        maximumPointCount: 3
        minimumScale: 0.1
        minimumRotation: 0
        maximumRotation: 0
        onActiveChanged: if (!active) {
                             // just a silly way of getting a QSize for sendConfigure()
                             var size = topLevel.sizeForResize(Qt.size(width * scale, height * scale), Qt.point(0, 0), 0);
                             topLevel.sendConfigure(size, [3] /*XdgShellToplevel.ResizingState*/);
                             rootChrome.scale = 1
                         }
    }

    Rectangle {
        z: surfaceItem.z + 9999999999
        visible: true
        border.color: "white"
        color: "black"
        radius: 5
        anchors.centerIn: parent
        width: height * 10
        height: moveGeometryText.implicitHeight * 1.5
        Text {
            id: moveGeometryText
            color: "white"
            anchors.centerIn: parent
            text: Math.round(rootChrome.x) + "," + Math.round(rootChrome.y) + " on " + rootChrome.screenName + "\n" + Math.round(surfaceItem.output.geometry.height) + "," + Math.round(rootChrome.height) + " ," + surfaceItem.output.cask.bottomPanel.height
        }

    }
}
