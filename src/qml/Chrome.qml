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
import QtWayland.Compositor 1.15
import QtGraphicalEffects 1.15
import com.theqtcompany.wlcompositor 1.0

StackableItem {
    id: rootChrome
    property alias shellSurface: surfaceItem.shellSurface
    property var topLevel
    property alias moveItem: surfaceItem.moveItem
    property bool decorationVisible: false
    property bool moving: surfaceItem.moveItem ? surfaceItem.moveItem.moving : false
    property alias destroyAnimation : destroyAnimationImpl

    property int marginWidth : surfaceItem.isFullscreen ? 0 : (surfaceItem.isPopup ? 1 : 6)
    property int titlebarHeight : surfaceItem.isPopup || surfaceItem.isFullscreen ? 0 : 25
    property string screenName: ""

    property real resizeAreaWidth: 12

    x: surfaceItem.moveItem.x - surfaceItem.output.geometry.x
    y: surfaceItem.moveItem.y - surfaceItem.output.geometry.y
    height: surfaceItem.height
    width: surfaceItem.width
    visible: surfaceItem.valid


    Rectangle {
        id: decoration
        anchors.fill: parent
        anchors.margins: 20
        radius: Maui.Style.radiusV
        color: "transparent"
        visible: true

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


    DropShadow
    {
        transparentBorder: true
        anchors.fill: decoration
        horizontalOffset: 0
        verticalOffset: 0
        radius: 8.0
        samples: 17
        color: "red"
        source: decoration
    }

    SequentialAnimation {
        id: destroyAnimationImpl
        ParallelAnimation {
            NumberAnimation { target: scaleTransform; property: "yScale"; to: 2/height; duration: 150 }
            NumberAnimation { target: scaleTransform; property: "xScale"; to: 0.4; duration: 150 }
        }
        NumberAnimation { target: scaleTransform; property: "xScale"; to: 0; duration: 150 }
        ScriptAction { script: { rootChrome.destroy(); } }
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

    ShellSurfaceItem {
        id: surfaceItem
        property bool valid: false
        property bool isPopup: false
        property bool isTransient: false
        property bool isFullscreen: true

//sizeFollowsSurface: surfaceItem.output.width < 500
        opacity: moving ? 0.5 : 1.0
        inputEventsEnabled: !pinch3.active && !metaDragHandler.active && !altDragHandler.active

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

        Connections {
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

            onSetMinimized:
            {
                surfaceItem.valid = false
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
