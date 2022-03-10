import QtQuick 2.12
import Qt.labs.settings 1.0
import Qt.labs.platform 1.1 as Labs

import org.mauikit.controls 1.3 as Maui

import QtWayland.Compositor 1.3
//import Liri.XWayland 1.0 as LXW

import org.maui.cask 1.0 as Cask
import Zpaces 1.0 as ZP
import org.cask.notifications 1.0 as Nof
import org.cask.polkit 1.0 as Polkit

WaylandCompositor
{
    id: comp

    Screen
    {
        id: desktop
        compositor: comp
        position: Qt.point(virtualX, virtualY)


    }

    Component
    {
        id: _polkitDialogComponent
        Polkit.PolkitDialog
        {
        }
    }

    Nof.Notifications
    {
        id: _notifications
    }

    Connections
    {
        target: _polkit.listener

        function onAuthenticationRequest(dialog)
        {
            var popup2 = _polkitDialogComponent.createObject(desktop.window, {"parent" : desktop.window, "dialog": dialog});
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
        showIsFullScreen: false
            onOpenUrl: Session.Launcher.launchCommand("xdg-open %1".arg(url))

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

    Settings
    {
        id: wallpaperSettings
        property bool dim: true
        property bool fill: true
        property string defaultWallpaper: "qrc:/wallpapers/maui_shell_dev_bg.png"
        property string sourcePath:  Labs.StandardPaths.writableLocation(Labs.StandardPaths.PicturesLocation) +"/Wallpapers"
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
