/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt-project.org/legal
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

import QtQuick 2.14
import QtQml 2.14
import QtQuick.Controls 2.14
import QtQuick.Window 2.3
import QtGraphicalEffects 1.0
import QtWayland.Compositor 1.0
import QtQuick.Layouts 1.3
import org.kde.mauikit 1.2 as Maui
import org.kde.kirigami 2.8 as Kirigami
import org.cask.env 1.0 as Env
import "shell"
import "shell/statusbar"
import "shell/tasksbar"

WaylandOutput
{
    id: output
    property alias surfaceArea: _cask.surface // Chrome instances are parented to compositorArea
    property alias targetScreen: win.screen
    property alias cask: _cask
    property alias swipeView : _swipeView

    property bool showDesktop : true
    sizeFollowsWindow: true
    availableGeometry : Qt.rect(surfaceArea.x, surfaceArea.y, surfaceArea.width, surfaceArea.height)
    //    scaleFactor: isMobile ? 2 : 1
//transform: WaylandOutput.Transform180
    readonly property bool isMobile : win.width < 500
    property bool overView: false

    window: Maui.ApplicationWindow
    {
        id: win
        visibility: Window.Windowed
        Maui.App.enableCSD: true
        color: "transparent"
        title: "Cask on " + Screen.name
        headBar.visible: false

        WaylandMouseTracker
        {
            id: mouseTracker
            objectName: "wmt on " + Screen.name
            anchors.fill: parent
            windowSystemCursorEnabled: false

            Cask
            {
                id: _cask
                anchors.fill: parent
                anchors.bottomMargin: 20
                rise: output.overView
                backgroundImage: "qrc:/calamares_wallpaper.jpg"
                bottomPanel.children: win.isWide ? [_taskBar, _statusBar] :  [_taskBar]
                topPanel.children: win.isWide ? [] :  [statusBar]
                bottomPanel.visible: win.isWide ? true : showDesktop || _listSurfaces.count <= 0

                readonly property QtObject statusBar : StatusBar {id: _statusBar}
                readonly property QtObject taskManagerBar: TaskBar {id: _taskBar}

                //                Rectangle
                //                {
                //                    color: "orange"
                //                    height: 64
                //                    width: 100
                //                    anchors.horizontalCenter: parent.horizontalCenter
                //                    anchors.bottom: parent.bottom
                //                    anchors.bottomMargin: height

                //                    Label
                //                    {
                //                        color: "white"
                //                        anchors.fill: parent
                //                        text: isMobile + " / " + win.width + " / " + Kirigami.Settings.isMobile
                //                    }
                //                }



                ListView
                {
                    id: _swipeView
                    anchors.fill: parent
                    visible: !showDesktop

                    Binding on currentIndex
                    {
                        delayed: true
                        value:  _appsOverview.currentIndex
                        restoreMode: Binding.RestoreBinding
                    }

                    onCurrentIndexChanged:
                    {
                        _swipeView.lastPos = _swipeView.contentX
                    }

                    orientation: ListView.Horizontal
                    interactive: false
                    snapMode: ListView.SnapOneItem
                    boundsBehavior: Flickable.StopAtBounds

                    highlightFollowsCurrentItem: true
                    highlightMoveDuration: 0
                    highlightResizeDuration: 0

//                    preferredHighlightEnd: width
                    // 		highlight: Item {}
                    highlightMoveVelocity: -1
                    highlightResizeVelocity: -1


                    property int lastPos: 0

                    Binding on contentX
                    {
                        when: overviewHandler.active
                        delayed: true
                        value: _swipeView.lastPos + ((overviewHandler.centroid.position.x - overviewHandler.centroid.pressPosition.x) * -1)
                        restoreMode: Binding.RestoreBinding
                    }

                    model: _listSurfaces
                    delegate: Chrome
                    {

                        id: _chromeDelegate
                        shellSurface: modelData
                        height: _swipeView.height
                        width: _swipeView.width
                        moveItem: Item
                        {
                            property bool moving: false
                            x: output.x
                            y: output.y
                            height: shellSurface.surface.height
                            width: shellSurface.surface.width
                        }

                        Connections
                        {
                            target: output.surfaceArea
                            onHeightChanged:  _chromeDelegate.shellSurface.toplevel.sendConfigure(Qt.size(desktop.availableGeometry.width, surfaceArea.height), [0])
//                            onWidthChanged:  _chromeDelegate.shellSurface.toplevel.sendConfigure(Qt.size(desktop.availableGeometry.width, desktop.availableGeometry.height), [0])
                        }
                    }

                }
            }

            Rectangle
            {
                height: 20
                opacity: 0.3
                width: parent.width
                color: "#333"
                anchors.bottom: parent.bottom

                Kirigami.Separator
                {
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right
                }

                MouseArea
                {
                    anchors.fill: parent
                    hoverEnabled: true

                    Rectangle
                    {
                        height: parent.height - 12
                        width: Math.min(100, parent.width * 0.5)
                        color: parent.containsMouse || parent.containsPress ? Kirigami.Theme.highlightColor : "white"
                        radius: 4
                        anchors.centerIn: parent
                    }

                }

                Label
                {
                    text: _swipeView.lastPos + " / " + _swipeView.contentX + " / " + _swipeView.currentIndex
                }

                DragHandler
                {
                    id: overviewHandler
                    target: null
                    onActiveChanged:
                    {
                        if(!active)
                        {
                            _swipeView.contentX += (overviewHandler.centroid.position.x - overviewHandler.centroid.pressPosition.x) * -1
                            _swipeView.returnToBounds()
                            _swipeView.currentIndex = _swipeView.indexAt(_swipeView.contentX, 0)

                            overView = Math.abs(overviewHandler.centroid.position.y) > 100 && _appsOverview.count > 0
                        }
                    }
                }
            }

            AppsOverview
            {
                id: _appsOverview
                anchors.fill: parent
            }

            WaylandCursorItem
            {
                id: cursor
                x: mouseTracker.mouseX
                y: mouseTracker.mouseY
                seat: output.compositor.defaultSeat
                visible: mouseTracker.containsMouse
            }
        }
    }
}
