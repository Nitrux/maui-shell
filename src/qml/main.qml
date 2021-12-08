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
import Qt.labs.settings 1.0
import org.mauikit.controls 1.3 as Maui

import QtWayland.Compositor 1.3
//import Liri.XWayland 1.0 as LXW

import org.maui.cask 1.0 as Cask
import Zpaces 1.0 as ZP

WaylandCompositor
{
    id: comp

    Screen
    {
        id: desktop
        compositor: comp
        position: Qt.point(virtualX, virtualY)
    }


    QtWindowManager
    {
        id: qtWindowManager
        onShowIsFullScreenChanged: console.debug("Show is fullscreen hint for Qt applications:", showIsFullScreen)
    }

//    WlShell {
//        onWlShellSurfaceCreated: handleShellSurfaceCreated(shellSurface, shellSurface)
//    }

    //    XdgShellV6
    //    {
    //        onToplevelCreated: handleShellSurfaceCreated(xdgSurface, toplevel)
    //    }



    XdgShell
    {
        // void toplevelCreated(QWaylandXdgToplevel *toplevel, QWaylandXdgSurface *xdgSurface);
        onToplevelCreated: handleShellSurfaceCreated(xdgSurface, toplevel)
    }

//    // Minimalistic shell extension. Mainly used for embedded applications.
//    IviApplication {
//        onIviSurfaceCreated: handleShellSurfaceCreated(iviSurface, iviSurface)
//    }

    XdgDecorationManagerV1
    {
        preferredMode: XdgToplevel.ServerSideDecoration
    }

    //    Component {
    //            id: shellSurfaceComponent

    //            LXW.XWaylandShellSurface {}
    //        }

    //    LXW.XWayland {
    //          id: xwayland

    //          enabled: true
    //          manager: LXW.XWaylandManager
    //          {
    //              id: manager
    //              onShellSurfaceRequested:
    //              {
    //                  var shellSurface = shellSurfaceComponent.createObject(manager);
    //                  shellSurface.initialize(manager, window, geometry, overrideRedirect, parentShellSurface);
    //              }
    //              onShellSurfaceCreated:
    //              {
    //                   handleShellSurfaceCreated(shellSurface, shellSurface)
    //              }
    //          }
    //          onServerStarted: {
    //              console.info("Xwayland server started");
    ////              Session.SessionManager.setEnvironment("DISPLAY", displayName);
    //          }
    //      }

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

    //    Component.onCompleted:
    //    {
    //        xwayland.startServer();
    //    }

    Component
    {
        id: _xdgWindowComponent
        ZP.XdgWindow
        {}
    }

    function handleShellSurfaceCreated(shellSurface, toplevel)
    {

        console.log(shellSurface.windowType)

var window = _xdgWindowComponent.createObject(null, {shellSurface: shellSurface, toplevel : toplevel})

        console.log("QML TOPLEVEL TYPE", shellSurface.title)

        desktop.workspaces.currentIndex = desktop.zpaces.addWindow(window);
        resizeSurface(shellSurface)

//        _zpaces.addSurface(shellSurface);
//        _listSurfaces.append({shellSurface: shellSurface})
        //        desktop.swipeView.currentIndex = _listSurfaces.count-1
        //        desktop.showDesktop = false
    }

    function resizeSurface(surface)
    {
        if(desktop.formFactor !== Cask.Env.Desktop)
        {
            surface.toplevel.sendConfigure(Qt.size(desktop.availableGeometry.width, desktop.cask.height-desktop.cask.topPanel.height), [1,3,4])
        }
    }
}
