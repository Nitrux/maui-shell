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
import QtQuick.Layouts 1.3

import QtQuick.Window 2.15

import QtGraphicalEffects 1.0

import QtWayland.Compositor 1.0

import org.mauikit.controls 1.2 as Maui
import org.kde.kirigami 2.8 as Kirigami
import org.maui.cask 1.0 as Cask

import "shell"
import "shell/statusbar"
import "shell/tasksbar"
import "shell/zpaces"

import "shell/statusbar/items/notifications"

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
    availableGeometry : _cask.availableGeometry

    scaleFactor: Screen.devicePixelRatio
    //transform: WaylandOutput.Transform180
    readonly property bool isMobile : formFactor === Cask.Env.Phone

    property alias overView:  _swipeView.overviewMode
    readonly property alias formFactor : win.formFactor
    property alias workspaces : _swipeView
    property alias zpaces : _zpaces
    property alias dock : _dock

    ZP.Zpaces
    {
        id: _zpaces
        output: control
    }

    window: Maui.ApplicationWindow
    {
        id: win
        width: 1024
        height: 760
        Maui.App.darkMode: true
        headBar.visible:false
        readonly property int formFactor :
        {
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
            windowSystemCursorEnabled: false

            //            ZP.WaylandCursorGrabber
            //            {
            //                seat: control.compositor.defaultSeat
            //                grab: mouseTracker.containsMouse
            //            }

            Cask.Dashboard
            {
                id: _cask
                clip: true
                anchors.fill: parent
                anchors.bottomMargin: _zpaceSwitcher.height
                topPanel.data: StatusBar {id: _statusBar}

                overlayContent: [
                    Dock
                    {
                        id: _dock
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.bottom: parent.bottom
                    },

                    NotificationsContainer
                    {
                        id: _overlayNotificationContainer
                        width: Math.min(availableGeometry.width, 300)
                        y: _cask.topPanel.height

                    }
                ]

                Shortcut
                {
                    sequence: "Meta+A" // maybe not the best one... or maybe we don't need it at all
                    onActivated: dock.launcher.toggle()
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

                WorkZpace
                {
                    id: _swipeView
                    height: parent.height
                    width: parent.width
                    model: _zpaces.zpacesModel
                    spacing: 20

                    delegate: Zpace
                    {
                        //                        scale: ListView.view.overviewScale
                        height: ListView.view.height
                        width: ListView.view.width
                        radius: ListView.view.overviewScale === 1 ? 0 : 20
                        clip: false
                        backgroundImage: wallpaperSettings.defaultWallpaper
                        overviewMode: overView
                        zpace : model.Zpace
                        topPadding: _cask.topPanel.height
                        bottomPadding: formFactor === Cask.Env.Desktop ? _dock.height : 0
                        leftPadding: 0
                        rightPadding: 0

                        onBackgroundImageChanged:
                        {
                            Maui.Style.adaptiveColorSchemeSource = wallpaperSettings.defaultWallpaper.replace("file://", "")
                        }

                        Item
                        {
                            id: _zpaceContainer
                            anchors.fill: parent

                            Repeater
                            {
                                model: zpace.windows

                                delegate: Chrome
                                {
                                    id: _chromeDelegate
                                    overviewMode: overView
                                    shellSurface: model.window.shellSurface
                                    window: model.window
                                    scale: isMobile ? 1  : _swipeView.scale
                                    moveItem: Item
                                    {
                                        property bool moving: false
                                        parent: _zpaceContainer

                                        height: _chromeDelegate.surface.size.height
                                        width: _chromeDelegate.surface.size.width
                                    }
                                }

                            }
                        }

                    }
                }


            }

            // Virtual Keyboard
            Loader
            {
                anchors.fill: parent
                active: Maui.Handy.isTouch
                source: "Keyboard.qml"
            }

            WorkZpaceSwitcher
            {
                id: _zpaceSwitcher
                visible: true

                workzpace: _swipeView
                height: visible ? 16 : 0
                width: parent.width
                anchors.bottom: parent.bottom
            }

            AppsOverview
            {
                id: _appsOverview
                anchors.fill: parent
                visible: opened
            }

            // Draws the mouse cursor for a given Wayland seat
            WaylandCursorItem {
                visible: false
//                inputEventsEnabled: false
                x: mouseTracker.mouseX
                y: mouseTracker.mouseY
                seat: control.compositor.defaultSeat
            }
        }
    }


    Component.onDestruction:
    {
        console.log("DESTROY SCREEN")
    }

}
