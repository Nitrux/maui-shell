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

import QtWayland.Compositor 1.3

import org.mauikit.controls 1.2 as Maui
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

    readonly property alias formFactor : win.formFactor

    readonly property bool isMobile : formFactor === Cask.Env.Phone

    //Orientations values
    //Qt.PortraitOrientation => 1
    //Qt.LandscapeOrientation => 2
    //Qt.InvertedPortraitOrientation => 4
    //Qt.InvertedLandscapeOrientation => 8
    //Qt.PrimaryOrientation => 0

    readonly property int primaryOrientation : Screen.primaryOrientation
    readonly property int orientation :
    {
        //                                            if(Screen.primaryOrientation === Screen.orientation)
        //                                            {
        //                                                return Screen.primaryOrientation;
        //                                            }

        switch(Cask.MauiMan.screen.orientation)
        {
        case 0: return Qt.PortraitOrientation // => 1
        case 1: return Qt.LandscapeOrientation // => 2
            //                                                case 2: return Qt.InvertedPortraitOrientation
            //                                                case 3: return Qt.InvertedLandscapeOrientation
        default: return Qt.PrimaryOrientation
        }
    }

    property alias surfaceArea: _cask.surface // Chrome instances are parented to compositorArea
    property alias targetScreen: win.screen
    property alias cask: _cask
    property alias overView:  _swipeView.overviewMode
    property alias workspaces : _swipeView
    property alias zpaces : _zpaces
    property alias dock : _dock

    property variant viewsBySurface: ({})

    property bool isNestedCompositor: Qt.platform.pluginName.startsWith("wayland") || Qt.platform.pluginName === "xcb"
    property bool showDesktop : true

    sizeFollowsWindow: true
    availableGeometry : _cask.availableGeometry

    scaleFactor: Cask.MauiMan.screen.scaleFactor
    transform: switch(control.orientation)
               {
               case Qt.PrimaryOrientation: return WaylandOutput.TransformNormal
               case Qt.PortraitOrientation: return WaylandOutput.Transform90
               case Qt.LandscapeOrientation: return WaylandOutput.Transform180
               case Qt.InvertedPortraitOrientation: return WaylandOutput.Transform270
               case Qt.InvertedLandscapeOrientation: return WaylandOutput.TransformFlipped
               default: return WaylandOutput.TransformNormal
               }


    ZP.Zpaces
    {
        id: _zpaces
        output: control
    }

    window: Window
    {
        id: win
        contentOrientation: switch(control.orientation)
                            {
                            case Qt.PrimaryOrientation: return Qt.PrimaryOrientation
                            case Qt.PortraitOrientation: return Qt.LandscapeOrientation
                            case Qt.LandscapeOrientation: return Qt.PortraitOrientation
                            case Qt.InvertedPortraitOrientation: return Qt.InvertedLandscapeOrientation
                            case Qt.InvertedLandscapeOrientation: return Qt.InvertedPortraitOrientation
                            }

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
            transform: Rotation
            {
                angle: switch(control.orientation)
                       {
                       case Qt.PrimaryOrientation: return 0;
                       case Qt.PortraitOrientation: return 90;
                       case Qt.LandscapeOrientation: return 0;
                       case Qt.InvertedPortraitOrientation: return 270;
                       case Qt.InvertedLandscapeOrientation: return 180;
                       default: return 0;
                       }

                origin.x: switch(control.orientation)
                          {
                          case Qt.PortraitOrientation:
                              return win.width / 2
                          case Qt.InvertedPortraitOrientation:
                              return win.height / 2
                          case Qt.InvertedLandscapeOrientation:
                              return win.width/2
                          }

                origin.y: switch(control.orientation)
                          {
                          case Qt.PortraitOrientation:
                              return win.width / 2
                          case Qt.InvertedPortraitOrientation:
                              return win.height / 2
                          case Qt.InvertedLandscapeOrientation:
                              return win.height/2
                          }
            }
            width: switch(control.orientation)
                   {
                   case Qt.PrimaryOrientation:
                   case Qt.LandscapeOrientation:
                   case Qt.InvertedLandscapeOrientation:
                       return win.width
                   default: win.height
                   }

            height: switch(control.orientation)
                    {
                    case Qt.PrimaryOrientation:
                    case Qt.LandscapeOrientation:
                    case Qt.InvertedLandscapeOrientation:
                        return win.height
                    default: win.width
                    }

            // Set this to false to disable the outer mouse cursor when running nested
            // compositors. Otherwise you would see two mouse cursors, one for each compositor.
            windowSystemCursorEnabled: false

            Cask.Dashboard
            {
                id: _cask
                clip: true
                backgroundColor: Cask.MauiMan.background.solidColor
                anchors.fill: parent
                anchors.bottomMargin: _zpaceSwitcher.height
                topPanel.data: StatusBar
                {
                    id: _statusBar
                    availableGeometry: _cask.availableGeometry
                }

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

                Keys.enabled: true
                Keys.onPressed: (event)=> {
                                    if (event.key == Qt.Key_Left) {
                                        console.log("move left");
                                        dock.launcher.toggle()

                                        event.accepted = true;
                                    }

                                    if((event.key === Qt.Key_Meta))
                                    {
                                        console.log("move meta");

                                        dock.launcher.toggle()
                                        event.accepted = true
                                    }
                                }
                Shortcut
                {
                    sequence: "Meta+A" // maybe not the best one... or maybe we don't need it at all
                    onActivated:
                    {
                        console.log("META AAAA")
                        dock.launcher.toggle()
                    }
                }

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
                        radius: ListView.view.overviewScale === 1 ? 0 : Maui.Style.radiusV
                        clip: false
                        backgroundVisible: Cask.MauiMan.background.showWallpaper
                        backgroundImage: Cask.MauiMan.background.wallpaperSource
                        backgroundFillMode: Cask.MauiMan.background.fitWallpaper ? Image.PreserveAspectFit : Image.PreserveAspectCrop
                        overviewMode: overView
                        zpace : model.Zpace
                        topPadding: _cask.topPanel.height
                        bottomPadding: formFactor === Cask.Env.Desktop ? _dock.height : 0
                        leftPadding: 0
                        rightPadding: 0

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
                                    parent: control.viewsBySurface[model.window.shellSurface.parentSurface] || _zpaceContainer
                                    overviewMode: overView
                                    shellSurface: model.window.shellSurface
                                    window: model.window
                                    scale: isMobile ? 1 : _swipeView.scale
                                    moveItem: Item
                                    {
                                        //                                        parent: control.surfaceArea
                                        property bool moving: false
                                        parent: _zpaceContainer

                                        height: _chromeDelegate.surface.height
                                        width: _chromeDelegate.surface.width
                                    }

                                    Component.onCompleted:
                                    {
                                        control.viewsBySurface[shellSurface.surface] = _chromeDelegate
                                    }
                                }
                            }
                        }
                    }
                }


                Rectangle
                {
                    color: "orange"
                    height: 64
                    width: 100
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: height

                    radius: Maui.Style.radiusV

                    Label
                    {
                        color: "#333"
                        width: parent.width
                        horizontalAlignment: Qt.AlignHCenter
                        font.bold: true
                        font.pointSize: Maui.Style.fontSizes.big
                        font.weight: Font.Bold
                        anchors.centerIn: parent
                        text: control.orientation + " / " + control.primaryOrientation
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
                //                inputEventsEnabled: false
                id: cursor
                x: mouseTracker.mouseX
                y: mouseTracker.mouseY
                seat: control.compositor.defaultSeat
                visible: mouseTracker.containsMouse
            }
        }
    }


    Component.onDestruction:
    {
        console.log("DESTROY SCREEN")
    }

}
