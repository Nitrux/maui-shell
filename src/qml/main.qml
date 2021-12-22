import QtQuick 2.12
import Qt.labs.settings 1.0
import org.mauikit.controls 1.3 as Maui

import QtWayland.Compositor 1.3
//import Liri.XWayland 1.0 as LXW

import org.maui.cask 1.0 as Cask
import Zpaces 1.0 as ZP
import org.cask.notifications 1.0 as Nof

WaylandCompositor
{
    id: comp

    Screen
    {
        id: desktop
        compositor: comp
        position: Qt.point(virtualX, virtualY)
    }

    Nof.Notifications
    {
        id: _notifications

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

    function handleShellSurfaceCreated(shellSurface, toplevel)
    {

        console.log(shellSurface.windowType)


        console.log("QML TOPLEVEL TYPE", shellSurface.title)
let window = desktop.zpaces.createXdgWindow(shellSurface, toplevel)
        desktop.workspaces.currentIndex = desktop.zpaces.addWindow(window, desktop.workspaces.currentIndex);
        if(desktop.formFactor !== Cask.Env.Desktop)
        {
        window.maximize()
        }
    }
}
