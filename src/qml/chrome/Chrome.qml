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
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import QtQml 2.15

import QtWayland.Compositor 1.15
import QtGraphicalEffects 1.15

import org.mauikit.controls 1.3 as Maui
import org.maui.cask 1.0 as Cask
import Zpaces 1.0 as ZP

Cask.StackableItem
{
    id: rootChrome

    objectName: "Chrome"

    //Check if an activated surface overlaps the dock
    readonly property bool intersects : toplevel ? (y+height > rootChrome.parent.height+20 || formFactor !== Cask.MauiMan.Desktop) && activated : false

    property alias shellSurface: surfaceItem.shellSurface

    property bool overviewMode : false

    property ZP.XdgWindow window
    property XdgSurface xdgSurface: window.xdgSurface
    property XdgToplevel toplevel: window.toplevel
    property WaylandSurface surface: xdgSurface.surface
    property WaylandSurface parentSurface: toplevel.parentToplevel.xdgSurface.surface

    readonly property string appId: window.appId
    readonly property string title: window.title

    readonly property bool activated : toplevel.activated
    property bool resizing : toplevel.resizing
    readonly property bool fullscreen : toplevel.fullscreen


    property alias moveItem: surfaceItem.moveItem

    readonly property bool decorationVisible: win.formFactor === Cask.MauiMan.Desktop && toplevel.decorationMode === XdgToplevel.ServerSideDecoration

    property bool moving: surfaceItem.moveItem ? surfaceItem.moveItem.moving : false

    readonly property alias titlebarHeight : _decoration.titleBarHeight
    property string screenName: ""

    property rect previousRect
    property rect oldPos : Qt.rect(0, 0, rootChrome.width * 0.6, rootChrome.height*0.6)

    x: surfaceItem.moveItem.x - surfaceItem.output.geometry.x
    y: surfaceItem.moveItem.y - surfaceItem.output.geometry.y

    //        Binding on x
    //        {
    //            when: rootChrome.moveItem
    //            value: surfaceItem.moveItem.x - surfaceItem.output.geometry.x
    //            restoreMode: Binding.RestoreBinding
    //        }

    //        Binding on y
    //        {
    //            when: rootChrome.moveItem
    //            value: surfaceItem.moveItem.y - surfaceItem.output.geometry.y
    //            restoreMode: Binding.RestoreBinding
    //        }

    height: surfaceItem.height + titlebarHeight
    width: surfaceItem.width

    visible: surfaceItem.valid && surfaceItem.paintEnabled

    onIntersectsChanged:
    {
        if(intersects)
            dock.hide()
        else
            dock.show()
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

        property bool valid: false
        property bool isPopup: false
        property bool isTransient: false

        objectName: "SurfaceItem-"+rootChrome.objectName

        y: titlebarHeight

        sizeFollowsSurface: false
        opacity: moving || pinch4.activeScale <= 0.5 ? 0.5 : 1.0

        inputEventsEnabled: !rootChrome.overviewMode
        touchEventsEnabled: !pinch3.active && !pinch4.active

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

        onWidthChanged:
        {
            valid =  !surface.cursorSurface && surface.size.width > 0 && surface.size.height > 0
        }

        layer.enabled: rootChrome.decorationVisible
        layer.effect: OpacityMask
        {
            maskSource: Maui.ShadowedRectangle
            {
                width: Math.floor(rootChrome.width)
                height: Math.floor(rootChrome.height)

                corners
                {
                    topLeftRadius: 0
                    topRightRadius: 0
                    bottomLeftRadius: _decoration.radius
                    bottomRightRadius: _decoration.radius
                }
            }
        }
    }

    Rectangle
    {
        id: _shield
        radius:  _decoration.radius
        visible: Cask.Server.screenshot.blacklisted.indexOf(rootChrome.appId) >= 0 && _screenshotArea.running
        color: Maui.Theme.backgroundColor
        anchors.fill: parent
        z: surfaceItem.z +1

        Maui.Holder
        {
            anchors.fill: parent
            emoji: "dialog-warning-symbolic"
            visible: true
            title: i18n("Protected")
            body: i18n("This app has requested to not be shown in screenshots.")
        }

    }

    Loader
    {
        asynchronous: true
        anchors.fill: parent
        active: win.formFactor === Cask.MauiMan.Desktop ? (rootChrome.decorationVisible && !window.maximized && !rootChrome.fullscreen) : (rootChrome.height < availableGeometry.height || rootChrome.width < availableGeometry.width || pinch4.active)
        z: surfaceItem.z +9999999999

        sourceComponent: Border {}
    }


    Connections
    {
        target: rootChrome.parent
        ignoreUnknownSignals: true
        enabled: win.formFactor !== Cask.MauiMan.Desktop

        function onWidthChanged()
        {
            if(formFactor !== Cask.MauiMan.Desktop)
            {
                window.maximize()
            }
        }

        function onHeightChanged()
        {
            if(formFactor !== Cask.MauiMan.Desktop)
            {
                window.maximize()
            }
        }
    }

    Connections
    {
        target: win
        ignoreUnknownSignals: true
        function onFormFactorChanged()
        {
            if(win.formFactor === Cask.MauiMan.Desktop)
            {
                rootChrome.shellSurface.toplevel.sendConfigure(Qt.size(previousRect.width, previousRect.height), [3,4])
                rootChrome.x = previousRect.x
                rootChrome.y = previousRect.y
            }else
            {
                previousRect = Qt.rect(rootChrome.x, rootChrome.y, rootChrome.width, rootChrome.height)
                rootChrome.x = 0
                rootChrome.y = 0
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
            focusTopWindow(1)
        }

        function onUnsetMinimized()
        {
            rootChrome.visible = true;
            surfaceItem.forceActiveFocus();
            window.activate()
        }

        function onSetMaximized()
        {
            console.log("SET MAX", toplevel.maximized, oldPos)

            oldPos.x = rootChrome.moveItem.x
            oldPos.y = rootChrome.moveItem.y
            oldPos.width = rootChrome.width
            oldPos.height = rootChrome.height

            rootChrome.x = 0
            rootChrome.y = 0

            rootChrome.moveItem.x =  rootChrome.x
            rootChrome.moveItem.y =  rootChrome.y

            toplevel.sendMaximized(Qt.size(rootChrome.parent.width, rootChrome.parent.height - rootChrome.titlebarHeight))
        }

        function onUnsetMaximized()
        {
            console.log("SET UNMAX", toplevel.maximized, oldPos)
            if(oldPos.width === -1)
            {
                oldPos = Qt.rect(0, 0, rootChrome.width * 0.6, rootChrome.height*0.6)
            }

            rootChrome.x = oldPos.x
            rootChrome.y = oldPos.y

            rootChrome.moveItem.x =  rootChrome.x
            rootChrome.moveItem.y =  rootChrome.y

            rootChrome.x = Qt.binding(()=> {return rootChrome.moveItem.x})
            rootChrome.y = Qt.binding(()=> {return rootChrome.moveItem.y})

            toplevel.sendUnmaximized(Qt.size(oldPos.width, oldPos.height))

        }

        //        function onSetFullscreen()
        //        {
        //            console.log("FUCK MY LIFE")

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
        //            console.log("FUCK MY LIFE")

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
