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
import QtQuick.Controls 2.14
import QtQuick.Window 2.3
import QtWayland.Compositor 1.0
import org.kde.mauikit 1.2 as Maui
import org.kde.kirigami 2.8 as Kirigami
import org.maui.cask 1.0 as Cask
import org.cask.env 1.0 as Env

WaylandOutput {
    id: output
    property variant viewsBySurface: ({})
    property alias surfaceArea: _cask.surface // Chrome instances are parented to compositorArea
    property alias targetScreen: win.screen
    property alias cask: _cask
    sizeFollowsWindow: true
    availableGeometry : Qt.rect(surfaceArea.x, surfaceArea.y, surfaceArea.width, surfaceArea.height)

    readonly property bool isMobile : win.width < 500

    window: Maui.ApplicationWindow {
        id: win
        visibility: Window.Windowed
        Maui.App.enableCSD: true
        color: "black"
        title: "Grefsen on " + Screen.name
        headBar.visible: false

        WaylandMouseTracker {
            id: mouseTracker
            objectName: "wmt on " + Screen.name
            anchors.fill: parent
            windowSystemCursorEnabled: false

            Cask.Cask
            {
                id: _cask
                anchors.fill: parent
                backgroundImage: "qrc:/calamares_wallpaper.jpg"
                bottomPanel.children: win.isWide ? [_taskBar, _statusBar] :  [_taskBar]
                topPanel.children: win.isWide ? [] :  [statusBar]

                readonly property QtObject statusBar : Cask.StatusBar {id: _statusBar}
                readonly property QtObject taskManagerBar: Cask.TaskBar {id: _taskBar}

                Rectangle
                {
                    color: "orange"
                    height: 64
                    width: 100
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: height

                    Label
                    {
                        color: "white"
                        anchors.fill: parent
                        text: isMobile + " / " + win.width + " / " + Kirigami.Settings.isMobile
                    }
                }
            }

            WaylandCursorItem {
                id: cursor
                x: mouseTracker.mouseX
                y: mouseTracker.mouseY
                seat: output.compositor.defaultSeat
                visible: mouseTracker.containsMouse
            }
        }
    }
}
