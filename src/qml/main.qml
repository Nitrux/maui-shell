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

import QtQuick 2.12
import QtWayland.Compositor 1.12
import Qt.labs.settings 1.0
import org.mauikit.controls 1.3 as Maui

import org.maui.cask 1.0 as Cask

WaylandCompositor
{
    id: comp

    Screen
    {
        id: desktop
        compositor: comp
        position: Qt.point(virtualX, virtualY)
    }

    ListModel {id: _listSurfaces }

    QtWindowManager
    {
        id: qtWindowManager
        onShowIsFullScreenChanged: console.debug("Show is fullscreen hint for Qt applications:", showIsFullScreen)
    }

    WlShell {
        onWlShellSurfaceCreated: handleShellSurfaceCreated(shellSurface, shellSurface, true)
    }

    XdgShellV6
    {
        onToplevelCreated: handleShellSurfaceCreated(xdgSurface, toplevel, true)
    }

    XdgShell
    {
        // void toplevelCreated(QWaylandXdgToplevel *toplevel, QWaylandXdgSurface *xdgSurface);
        onToplevelCreated: handleShellSurfaceCreated(xdgSurface, toplevel, true)

    }

    XdgDecorationManagerV1
    {
        preferredMode: XdgToplevel.ServerSideDecoration
    }

    TextInputManager { }

    defaultSeat.keymap
    {
        layout: keymapSettings.layout
        variant: keymapSettings.variant
        options: keymapSettings.options
        rules: keymapSettings.rules
        model: keymapSettings.model
    }

    Settings
    {
        id: keymapSettings
        category: "keymap"
        property string layout: "us"
        property string variant: "intl"
        property string options: "grp:shifts_toggle,compose:ralt,ctrl:nocaps"
        property string rules: ""
        property string model: ""
    }

    function handleShellSurfaceCreated(shellSurface, topLevel, decorate)
    {
        if(desktop.formFactor !== Cask.Env.Desktop)
            shellSurface.toplevel.sendConfigure(Qt.size(desktop.availableGeometry.width, desktop.availableGeometry.height), [0])

        _listSurfaces.append({shellSurface: shellSurface})
        desktop.swipeView.currentIndex = _listSurfaces.count-1
        desktop.showDesktop = false
    }
}
