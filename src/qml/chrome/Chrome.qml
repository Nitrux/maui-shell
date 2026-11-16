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

import QtQuick
import QtQml

import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects

import QtWayland.Compositor
import QtWayland.Compositor.XdgShell

import org.mauikit.controls as Maui
import org.maui.cask as Cask
import Zpaces as ZP

Cask.StackableItem
{
    id: rootChrome

    objectName: "Chrome"

    //Check if an activated surface overlaps the dock
    readonly property bool intersects : toplevel ? (y+height > rootChrome.parent.height+20 || CaskShell.formFactor !== Cask.MauiMan.Desktop) && activated : false

    property alias shellSurface: surfaceItem.shellSurface

    property bool overviewMode : false

    property ZP.XdgWindow window
    property XdgSurface xdgSurface: window ? window.xdgSurface : null
    property XdgToplevel toplevel: window ? window.toplevel : null
    property WaylandSurface surface: xdgSurface ? xdgSurface.surface : null
    property WaylandSurface parentSurface: toplevel && toplevel.parentToplevel ? toplevel.parentToplevel.xdgSurface.surface : null

    // The Zpace delegate this chrome lives in, used to focus another window.
    property var zpaceItem

    readonly property string appId: window ? window.appId : ""
    readonly property string title: window ? window.title : ""

    readonly property bool activated : toplevel ? toplevel.activated : false
    property bool resizing : toplevel ? toplevel.resizing : false
    readonly property bool fullscreen : toplevel ? toplevel.fullscreen : false

    property alias moveItem: surfaceItem.moveItem

    readonly property bool decorationVisible: CaskShell.formFactor === Cask.MauiMan.Desktop && toplevel && toplevel.decorationMode === XdgToplevel.ServerSideDecoration

    property bool moving: surfaceItem.moveItem ? surfaceItem.moveItem.moving : false

    readonly property alias titlebarHeight : _decoration.titleBarHeight
    property string screenName: ""

    property rect previousRect
    property rect oldPos : Qt.rect(0, 0, rootChrome.width * 0.6, rootChrome.height*0.6)

    x: surfaceItem.moveItem.x
    y: surfaceItem.moveItem.y

    height: surfaceItem.height + titlebarHeight
    width: surfaceItem.width

    visible: surfaceItem.valid && surfaceItem.paintEnabled

    onIntersectsChanged:
    {
        if(!CaskShell.dock)
            return

        if(intersects)
            CaskShell.dock.hide()
        else
            CaskShell.dock.show()
    }

    transform: [
        Scale
        {
            id: scaleTransform
            origin.x: rootChrome.width / 2
            origin.y: rootChrome.height / 2
        }
    ]

    Decoration
    {
        id: _decoration
        anchors.fill: parent

        chrome: rootChrome
        window: rootChrome.window

        showTitleBar: rootChrome.decorationVisible && !rootChrome.fullscreen
        showResizeHandlers: rootChrome.decorationVisible && !rootChrome.fullscreen
        showDropShadow: !window.maximized && !rootChrome.fullscreen
        canResize: !window.maximized && rootChrome.activated && !rootChrome.fullscreen

        onRequestResize:
        {
            toplevel.sendResizing(size)
        }
    }

    ShellSurfaceItem
    {
        id: surfaceItem

        // WaylandSurface has no `size` property in Qt 6; the size the
        // compositor uses is `destinationSize`. Keep this as a binding so it
        // also updates when the client resizes, instead of only on width
        // changes (the old handler threw a TypeError and left this false).
        readonly property bool valid: surface !== null
                                       && !surface.cursorSurface
                                       && surface.destinationSize.width > 0
                                       && surface.destinationSize.height > 0
        property bool isPopup: false
        property bool isTransient: false

        objectName: "SurfaceItem-"+rootChrome.objectName

        y: titlebarHeight

        //        sizeFollowsSurface: false
        opacity: moving || pinch4.activeScale <= 0.5 ? 0.5 : 1.0

        inputEventsEnabled: !rootChrome.overviewMode
        touchEventsEnabled: !pinch4.active
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

        DragHandler
        {
            id: metaDragHandler
            acceptedModifiers: Qt.MetaModifier
            target: surfaceItem.moveItem

            property var movingBinding: Binding
            {
                target: surfaceItem.moveItem
                property: "moving"
                value: metaDragHandler.active
            }
        }

        DragHandler
        {
            id: altDragHandler
            acceptedModifiers: Qt.AltModifier
            target: surfaceItem.moveItem

            property var movingBinding: Binding
            {
                target: surfaceItem.moveItem
                property: "moving"
                value: altDragHandler.active
            }
        }

        // layer.enabled: rootChrome.decorationVisible
        // layer.effect: MultiEffect
        // {
        //     maskEnabled: true
        //     maskThresholdMin: 0.5
        //     maskSpreadAtMin: 1.0
        //     maskSpreadAtMax: 0.0
        //     maskThresholdMax: 1.0
        //     maskSource: ShaderEffectSource
        //     {
        //         sourceItem: Maui.ShadowedRectangle
        //         {
        //             width: Math.floor(rootChrome.width)
        //             height: Math.floor(rootChrome.height)

        //             corners
        //             {
        //                 topLeftRadius: 0
        //                 topRightRadius: 0
        //                 bottomLeftRadius: _decoration.radius
        //                 bottomRightRadius: _decoration.radius
        //             }
        //         }
        //     }
        // }
    }

    Loader
    {
        id:  _shieldLoader
        anchors.fill: parent
        z: surfaceItem.z +1

        sourceComponent: Rectangle
        {
            radius:  _decoration.radius
            visible: Cask.Server.screenshot.blacklisted.indexOf(rootChrome.appId) >= 0 && CaskShell.screenshot && CaskShell.screenshot.running
            color: Maui.Theme.backgroundColor

            Maui.Holder
            {
                anchors.fill: parent
                emoji: "dialog-warning-symbolic"
                visible: true
                title: i18n("Protected")
                body: i18n("This app has requested to not be shown in screenshots.")
            }
        }
    }

    Loader
    {
        asynchronous: true
        anchors.fill: parent
        active: CaskShell.formFactor === Cask.MauiMan.Desktop ? (rootChrome.decorationVisible && !window.maximized && !rootChrome.fullscreen) : (rootChrome.height < CaskShell.availableGeometry.height || rootChrome.width < CaskShell.availableGeometry.width || pinch4.active)
        z: surfaceItem.z + 9999999999

        sourceComponent: Border { decoration: _decoration }
    }

    Connections
    {
        target: rootChrome.parent
        ignoreUnknownSignals: true
        enabled: CaskShell.formFactor !== Cask.MauiMan.Desktop

        function onWidthChanged()
        {
            if(CaskShell.formFactor !== Cask.MauiMan.Desktop)
            {
                window.maximize()
            }
        }

        function onHeightChanged()
        {
            if(CaskShell.formFactor !== Cask.MauiMan.Desktop)
            {
                window.maximize()
            }
        }
    }

    Connections
    {
        target: CaskShell.win
        ignoreUnknownSignals: true
        function onFormFactorChanged()
        {
            if(CaskShell.formFactor === Cask.MauiMan.Desktop)
            {
                rootChrome.shellSurface.toplevel.sendConfigure(Qt.size(previousRect.width, previousRect.height), [3,4])
                rootChrome.moveItem.x = previousRect.x
                rootChrome.moveItem.y = previousRect.y
            }else
            {
                previousRect = Qt.rect(rootChrome.x, rootChrome.y, rootChrome.width, rootChrome.height)
                rootChrome.moveItem.x = 0
                rootChrome.moveItem.y = 0
            }
        }
    }

    Connections
    {
        target: rootChrome.window

        // xdg_shell only
        function onActivatedChanged ()
        {
            if (activated)
            {
                surfaceItem.forceActiveFocus()
            }
        }

        function onSetMinimized()
        {
            rootChrome.visible = false
            window.deactivate()
            if(zpaceItem)
                zpaceItem.focusTopWindow(1)
        }

        function onUnsetMinimized()
        {
            rootChrome.visible = true
            surfaceItem.forceActiveFocus()
            window.activate()
        }

        function onSetMaximized()
        {

            oldPos.x = rootChrome.moveItem.x
            oldPos.y = rootChrome.moveItem.y
            oldPos.width = rootChrome.width
            oldPos.height = rootChrome.height

            // rootChrome.x = 0
            // rootChrome.y = 0

            rootChrome.moveItem.x =  0
            rootChrome.moveItem.y =  0

            toplevel.sendMaximized(Qt.size(rootChrome.parent.width, rootChrome.parent.height - rootChrome.titlebarHeight))
        }

        function onUnsetMaximized()
        {
            if(oldPos.width === -1)
            {
                oldPos = Qt.rect(0, 0, rootChrome.width * 0.6, rootChrome.height*0.6)
            }
            toplevel.sendUnmaximized(Qt.size(oldPos.width, oldPos.height))

            rootChrome.x = Qt.binding(()=> {return rootChrome.moveItem.x})
            rootChrome.y = Qt.binding(()=> {return rootChrome.moveItem.y})

            rootChrome.moveItem.x =  oldPos.x
            rootChrome.moveItem.y =  oldPos.y
        }

        //        function onSetFullscreen()
        //        {

        //            oldPos.x = rootChrome.x
        //            oldPos.y = rootChrome.y
        //            oldPos.width = rootChrome.width
        //            oldPos.height = rootChrome.height

        //            rootChrome.x = 0
        //            rootChrome.y = 0

        //            toplevel.sendFullscreen(Qt.size(container.width, container.height))
        //        }


        //        function onUnsetFullscreen()
        //        {

        //            rootChrome.x = oldPos.x
        //            rootChrome.y = oldPos.y
        //            toplevel.sendConfigure(Qt.size(oldPos.width, oldPos.height), [3,4])
        //        }
    }

    PinchArea
    {
        id: pinch4
        objectName: "4-finger pinch"
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

    Component.onCompleted:
    {
        rootChrome.window.chrome = rootChrome
        surfaceItem.forceActiveFocus()
        rootChrome.visible = true
    }

    Rectangle
    {
        z: surfaceItem.z + 9999999999
        visible: false
        border.color: "white"
        color: "black"
        radius: Maui.Style.radiusV
        anchors.centerIn: parent
        width: height * 10
        height: moveGeometryText.implicitHeight * 1.5
        Text {
            id: moveGeometryText
            color: "white"
            anchors.centerIn: parent
            //                text: Math.round(rootChrome.x) + "," + Math.round(rootChrome.y) + " on " + rootChrome.screenName + "\n" + Math.round(surfaceItem.output.geometry.height) + "," + Math.round(rootChrome.height) + " ," + rootChrome.scale + " / " + pinch4.activeScale
            //            text: rootChrome.parent.objectName
            //                text:  rootChrome.appId + " / " + toplevel.maximized
            text:  rootChrome.moveItem.x + " / " + rootChrome.moveItem.y + " - " + rootChrome.x + " / " + rootChrome.y

        }
    }
}
