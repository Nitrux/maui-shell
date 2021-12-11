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

import QtQuick 2.15
import QtQml 2.14
import QtQuick.Controls 2.14
import QtQuick.Window 2.3
import QtGraphicalEffects 1.0
import QtWayland.Compositor 1.0
import QtQuick.Layouts 1.3
import org.mauikit.controls 1.2 as Maui
import org.kde.kirigami 2.8 as Kirigami
import org.maui.cask 1.0 as Cask
import "shell"
import "shell/statusbar"
import "shell/tasksbar"

import Zpaces 1.0 as ZP

WaylandOutput
{
    id: control

    property bool isNestedCompositor: Qt.platform.pluginName.startsWith("wayland") || Qt.platform.pluginName === "xcb"
    property alias surfaceArea: _cask.surface // Chrome instances are parented to compositorArea
    property alias targetScreen: win.screen
    property alias cask: _cask
    //    property alias swipeView : _swipeView

    property bool showDesktop : true

    sizeFollowsWindow: isNestedCompositor
    availableGeometry : Qt.rect(surfaceArea.x, surfaceArea.y, surfaceArea.width, surfaceArea.height)

    scaleFactor: 1
    //transform: WaylandOutput.Transform180
    readonly property bool isMobile : formFactor === Cask.Env.Phone

    property bool overView: false
    readonly property alias formFactor : win.formFactor
    property alias workspaces : _swipeView
    property alias zpaces : _zpaces

    ZP.Zpaces
    {
        id: _zpaces
        output: control
    }

    window: Window
    {
        id: win

        readonly property int formFactor :  {
            if(width > 1500)
                return Cask.Env.Desktop
            else if(width > 500)
                return Cask.Env.Tablet
            else
                return Cask.Env.Phone
        }


        WaylandMouseTracker
        {
            id: mouseTracker
            objectName: "wmt on " + Screen.name
            anchors.fill: parent
            // Set this to false to disable the outer mouse cursor when running nested
            // compositors. Otherwise you would see two mouse cursors, one for each compositor.
            windowSystemCursorEnabled: mouseTracker.containsMouse

            ZP.WaylandCursorGrabber
            {
                seat: control.compositor.defaultSeat
                grab: mouseTracker.containsMouse
            }

            Cask.Dashboard
            {
                id: _cask
                clip: true
                anchors.fill: parent
                backgroundImage: "qrc:/calamares_wallpaper.jpg"

                property alias dock : _taskBar

                bottomPanel.data: TaskBar {id: _taskBar}
                topPanel.data: StatusBar {id: _statusBar}

                Shortcut
                {
                    sequence: "Meta+A" // maybe not the best one... or maybe we don't need it at all
                    onActivated: _taskBar.launcher.toggleLauncherPopup()
                }


//                Rectangle
//                {
//                    color: "orange"
//                    height: 64
//                    width: 100
//                    anchors.horizontalCenter: parent.horizontalCenter
//                    anchors.bottom: parent.bottom
//                    anchors.bottomMargin: height

//                    radius: Maui.Style.radiusV

//                    Label
//                    {
//                        color: "#333"
//                        width: parent.width
//                        horizontalAlignment: Qt.AlignHCenter
//                        font.bold: true
//                        font.pointSize: Maui.Style.fontSizes.big
//                        font.weight: Font.Bold
//                        anchors.centerIn: parent
//                        text: control.surfaceArea.x + " / " + control.surfaceArea.y + " - " + control.surfaceArea.width + " / " + control.surfaceArea.height
//                    }
//                }

                ListView
                {
                    id: _swipeView

                    anchors.fill: parent
                    clip: false

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
                    cacheBuffer: count * width
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
//                    Label
//                    {
//                        text: _swipeView.count + "/" + _zpaces.zpacesModel.count
//                        color: "orange"
//                        font.bold: true
//                        font.pointSize: 22
//                    }

                    model: _zpaces.zpacesModel

                    Connections
                    {
                        target: _zpaces.zpacesModel
                        ignoreUnknownSignals: true
                        function onZpaceAdded()
                        {
                            _swipeView.incrementCurrentIndex()
                        }
                    }

                    delegate: Item
                    {
                        height: ListView.view.height
                        width: ListView.view.width

                        clip: false

                        property ZP.Zpace zpace : model.Zpace


//                        Label
//                        {
//                            text: zpace.limit
//                            color: "orange"
//                            font.bold: true
//                            font.pointSize: 22
//                            anchors.right: parent.right
//                        }

                        Repeater
                        {
                            model: zpace.windows

                            delegate: Chrome
                            {
                                id: _chromeDelegate
                                shellSurface: model.window.shellSurface
                                window: model.window
                                moveItem: Item
                                {
                                    property bool moving: false
                                    parent: surfaceArea
                                    x: control.position.x
                                    y: control.position.y
                                    height: _chromeDelegate.surface.destinationSize.height
                                    width: _chromeDelegate.surface.destinationSize.width
                                }
                            }

                        }

                    }
                }
            }

            // Virtual Keyboard
            Loader {
                anchors.fill: parent
                source: "Keyboard.qml"
            }

            Rectangle
            {
                id: _swiper
                visible:  _swipeView.count > 1
                height: visible ? 16 : 0
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

                HoverHandler
                {
                    id: _hadleHandler
                    cursorShape: Qt.OpenHandCursor
                }

                Rectangle
                {
                    x: ((parent.width/2) - width/2) - ((overviewHandler.centroid.position.x - overviewHandler.centroid.pressPosition.x) * -1)
                    anchors.verticalCenter: parent.verticalCenter
                    height: parent.height - 12
                    width: Math.min(100, parent.width * 0.5)
                    color: _hadleHandler.hovered ? Kirigami.Theme.highlightColor : "white"
                    radius: 4
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

//             WaylandCursorItem
//             {
//                 id: cursor
//                 inputEventsEnabled: false
//                 x: mouseTracker.mouseX
//                 y: mouseTracker.mouseY
//                 seat: control.compositor.defaultSeat
//             }
        }
    }


    Component.onDestruction:
    {
        console.log("DESTROY SCREEN")
    }

}
