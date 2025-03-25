import QtQuick

import QtWayland.Compositor
import QtWayland.Compositor.XdgShell
import QtWayland.Compositor.WlShell

import org.mauikit.controls as Maui

import Zpaces as ZP

import org.maui.cask as Cask
import org.mauicore.notifications as Nof
import org.cask.polkit as Polkit

//import Liri.XWayland 1.0 as LXW

WaylandCompositor
{
    id: comp
    retainedSelection: true
    useHardwareIntegrationExtension: true

    CaskScreen
    {
        id: screen
        compositor: comp
//        targetScreen: modelData
        position: Qt.point(virtualX, virtualY)

        Component.onCompleted:
        {
            if (!comp.defaultOutput)
                comp.defaultOutput = this
        }
    }

    Nof.Notifications
    {
        id: _notifications
    }

    Component
    {
        id: _polkitDialogComponent
        Polkit.PolkitDialog {}
    }

    Connections
    {
        target: _polkit.listener

        function onAuthenticationRequest(dialog)
        {
            var popup2 = _polkitDialogComponent.createObject(comp.defaultOutput.window, {"parent" : comp.defaultOutput.window, "dialog": dialog});
            popup2.open()
        }
    }

    Polkit.Agent
    {
        id: _polkit
    }

    QtWindowManager
    {
        id: qtWindowManager
        onShowIsFullScreenChanged: console.debug("Show is fullscreen hint for Qt applications:", showIsFullScreen)
        //        showIsFullScreen: false
        onOpenUrl:
        {
            console.log("Ask to open url", url)
            Session.Launcher.launchCommand("xdg-open %1".arg(url))
        }
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

    XdgDecorationManagerV1
    {
        preferredMode: XdgToplevel.ServerSideDecoration
    }

    TextInputManager { }

    defaultSeat.keymap
    {
        layout: Cask.MauiMan.inputDevices.keyboardLayout
        variant: Cask.MauiMan.inputDevices.keyboardVariant
        options:  Cask.MauiMan.inputDevices.keyboardOptions
        rules:  Cask.MauiMan.inputDevices.keyboardRules
        model:  Cask.MauiMan.inputDevices.keyboardModel
    }

    Connections
    {
        target: defaultSeat
        function onCursorSurfaceRequest()
        {
            console.log("CURSOR SURFACE REQUEST"/*,hotspotX, hotspotY, surface.cursorSurface, surface.hasContent, surface.inhibitsIdle*/)
        }

        function onKeyboardFocusChanged()
        {
            console.log("KEYBOARD FOCUS REQUEST")
        }

        function onMouseFocusChanged()
        {
            console.log("MOUSE FOCUS REQUEST")
        }
    }

//    Connections
//    {
//        target: comp.drag
//        function onDragStarted()
//        {
//            console.log("DRAG STARTED")
//        }

//        function onIconChanged()
//        {
//            console.log("DRAG ICON CHANGED")
//        }
//    }

    function handleShellSurfaceCreated(shellSurface, toplevel)
    {
        console.log(shellSurface.windowType)

        let window = screen.zpaces.createXdgWindow(shellSurface, toplevel)

        screen.workspaces.currentIndex = screen.zpaces.addWindow(window, screen.workspaces.currentIndex)
        window.activate()

        if(screen.formFactor !== Cask.MauiMan.Desktop)
        {
            window.maximize()
        }

    }
}
