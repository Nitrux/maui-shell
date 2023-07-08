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
import "chrome"

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

    // mapped values
    //0 => Qt.PortraitOrientation
    //1 => Qt.LandscapeOrientation
    readonly property int orientation :
    {
        if(Maui.Handy.isMobile)
        {
            return Qt.PrimaryOrientation;
        }

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
               case Qt.PortraitOrientation: return (control.primaryOrientation === Qt.PortraitOrientation ? WaylandOutput.TransformNormal : WaylandOutput.Transform90)
               case Qt.LandscapeOrientation: return (control.primaryOrientation === Qt.LandscapeOrientation ? WaylandOutput.TransformNormal : WaylandOutput.Transform90)
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
            onActiveFocusItemChanged: print("activeFocusItem", activeFocusItem)
        contentOrientation: switch(control.orientation)
                            {
                            case Qt.PrimaryOrientation: return Qt.PrimaryOrientation
                            case Qt.PortraitOrientation: return (control.primaryOrientation === Qt.PortraitOrientation ? control.orientation : Qt.LandscapeOrientation)
                            case Qt.LandscapeOrientation: return (control.primaryOrientation === Qt.LandscapeOrientation ? Qt.PortraitOrientation : Qt.LandscapeOrientation)
                            case Qt.InvertedPortraitOrientation: return Qt.InvertedLandscapeOrientation
                            case Qt.InvertedLandscapeOrientation: return Qt.InvertedPortraitOrientation
                            }

        readonly property int formFactor : Cask.MauiMan.formFactor.preferredMode

        WaylandMouseTracker
        {
            id: mouseTracker
            objectName: "wmt on " + Screen.name
            transform: Rotation
            {
                angle: switch(control.orientation)
                       {
                       case Qt.PrimaryOrientation: return 0;
                       case Qt.PortraitOrientation: return (control.primaryOrientation === Qt.PortraitOrientation ? 0 : 90);
                       case Qt.LandscapeOrientation: return (control.primaryOrientation === Qt.LandscapeOrientation ? 0 : 90);
                       case Qt.InvertedPortraitOrientation: return 270;
                       case Qt.InvertedLandscapeOrientation: return 180;
                       default: return 0;
                       }

                origin.x: switch(control.orientation)
                          {
                          case Qt.PrimaryOrientation: return 0;

                          case Qt.PortraitOrientation:
                          case Qt.LandscapeOrientation: return win.width / 2

                          case Qt.InvertedPortraitOrientation:
                          case Qt.InvertedLandscapeOrientation: return win.height/2
                          }

                origin.y: switch(control.orientation)
                          {
                          case Qt.PrimaryOrientation: return 0;

                          case Qt.PortraitOrientation:
                          case Qt.LandscapeOrientation: return win.width / 2

                          case Qt.InvertedPortraitOrientation:
                          case Qt.InvertedLandscapeOrientation: return win.height/2
                          }
            }

            width: switch(control.orientation)
                   {
                   case Qt.LandscapeOrientation:
                   case Qt.InvertedLandscapeOrientation: return (control.primaryOrientation === Qt.LandscapeOrientation? win.width : win.height)
                   case Qt.InvertedPortraitOrientation:
                   case Qt.PortraitOrientation: return (control.aZXCVB === Qt.PortraitOrientation? win.width : win.height)
                   default: return win.width
                   }

            height: switch(control.orientation)
                    {
                    case Qt.LandscapeOrientation:
                    case Qt.InvertedLandscapeOrientation: return (control.primaryOrientation === Qt.LandscapeOrientation? win.height : win.width)
                    case Qt.InvertedPortraitOrientation:
                    case Qt.PortraitOrientation:  return (control.primaryOrientation === Qt.PortraitOrientation? win.height : win.width)
                    default: return win.height

                    }

            // Set this to false to disable the outer mouse cursor when running nested
            // compositors. Otherwise you would see two mouse cursors, one for each compositor.
            windowSystemCursorEnabled: false

            Cask.Dashboard
            {
                id: _cask
//                clip: true
                backgroundColor: Cask.MauiMan.aZXCVB.solidColor
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
                    }/*,

                    OSDArea
                    {
                        id: _osdArea
                        anchors.topMargin: _cask.topPanel.height
                        anchors.top: parent.top
                        anchors.horizontalCenter: parent.horizontalCenter
                        width: 600
                        height: implicitHeight
                    }*/
                ]

                Keys.enabled: true
                Keys.onPressed: {
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

                Shortcut
                {
                    sequence: "Alt+Q" // maybe not the best one... or maybe we don't need it at all
                    onActivated:
                    {
                        console.log("TABBING")
                        _zpaces.allSurfaces.activateNextWindow()
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
                        id: _zpaceDelegate
                        objectName: "Zpace-"+index

                        height: ListView.view.height
                        width: ListView.view.width
                        radius: ListView.view.overviewScale === 1 ? 0 : Maui.Style.radiusV

                        backgroundVisible: Cask.MauiMan.background.showWallpaper
                        backgroundImage: Cask.MauiMan.background.wallpaperSource
                        backgroundFillMode: Cask.MauiMan.background.fitWallpaper ? Image.PreserveAspectFit : Image.PreserveAspectCrop

                        overviewMode: control.overView

                        zpace : model.Zpace

                        function forceActiveFocus()
                        {
                            _zpaceDelegate.zpace.windows.currentWindow.activate()
                        }
                    }
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
////                        text: control.orientation + " / " + control.primaryOrientation + " / " + Screen.orientation
//                        text: win.activeFocusItem.objectName
//                    }
//                }
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
                visible: Maui.Handy.isTouch

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

            ScreenshotArea
            {
                id: _screenshotArea
                anchors.fill: parent
            }

            // Draws the mouse cursor for a given Wayland seat
            WaylandCursorItem
            {
                                inputEventsEnabled: false
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
