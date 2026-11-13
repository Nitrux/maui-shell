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
        position: Qt.point(0, 0)

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

    Binding { target: CaskShell; property: "notifications"; value: _notifications }

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
            var popup2 = _polkitDialogComponent.createObject(comp.defaultOutput.window.contentItem, {"parent" : comp.defaultOutput.window.contentItem, "dialog": dialog});
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
        //        showIsFullScreen: false
        onOpenUrl:
        {
            Qt.openUrlExternally(url)
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
        onToplevelCreated: (toplevel, xdgSurface) => handleShellSurfaceCreated(xdgSurface, toplevel)
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
        }

        function onKeyboardFocusChanged()
        {
        }

        function onMouseFocusChanged()
        {
        }
    }

//    Connections
//    {
//        target: comp.drag
//        function onDragStarted()
//        {
//        }

//        function onIconChanged()
//        {
//        }
//    }

    function handleShellSurfaceCreated(shellSurface, toplevel)
    {

        let window = screen.zpaces.createXdgWindow(shellSurface, toplevel)

        screen.workspaces.currentIndex = screen.zpaces.addWindow(window, screen.workspaces.currentIndex)
        window.activate()

        if(screen.formFactor !== Cask.MauiMan.Desktop)
        {
            window.maximize()
        }

    }
}
