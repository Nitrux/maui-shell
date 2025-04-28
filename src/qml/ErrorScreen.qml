/****************************************************************************
 * *
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

import QtQuick
import QtQml
import QtQuick.Controls
import QtQuick.Layouts

import QtQuick.Window

import QtWayland.Compositor

import org.mauikit.controls as Maui

WaylandCompositor
{
    id: comp

WaylandOutput
{
    id: output
    compositor: comp
    Component.onCompleted:
    {
        if (!comp.defaultOutput)
            comp.defaultOutput = this
    }

    window: Window
    {
        id: outputWindow

        x: output.position.x
        y: output.position.y
        width: output.geometry.width
        height: output.geometry.height
        flags: Qt.Window | Qt.FramelessWindowHint
        screen: output.screen ? Qt.application.screens[output.screen.screenIndex] : null
        color: "black"
        visible: true

        WaylandMouseTracker
        {
            id: mouseTracker

            anchors.fill: parent
            windowSystemCursorEnabled: containsMouse

            Rectangle {
                id: screenView
                anchors.fill: parent
                color: "red"

                Maui.Holder
                {
                    anchors.fill: parent
                    title: i18n("Error")
                    body: "The shell failed to be loaded"
                    emoji: "dialog-error-symbolic"
                    Action
                    {
                        text: i18n("Quit")
                    }
                }
            }

            WaylandCursorItem {
                seat: output.compositor.defaultSeat
                // grab: mouseTracker.containsMouse
            }
        }
    }
}

}
