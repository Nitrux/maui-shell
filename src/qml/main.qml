import QtQuick 2.15

import QtWayland.Compositor 1.3

import Qt.labs.settings 1.0
import Qt.labs.platform 1.1 as Labs

import org.mauikit.controls 1.3 as Maui

import Zpaces 1.0 as ZP

import org.maui.cask 1.0 as Cask
import org.cask.notifications 1.0 as Nof
import org.cask.polkit 1.0 as Polkit

//import Liri.XWayland 1.0 as LXW

WaylandCompositor
{
    id: comp
    //    retainedSelection: true
    //    useHardwareIntegrationExtension: true

    Screen
    {
        id: screen
        compositor: comp
        targetScreen: modelData
        Component.onCompleted: if (!comp.defaultOutput) comp.defaultOutput = this
        position: Qt.point(virtualX, virtualY)
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
        id: shellSettings
        category: "Cask"
        property int scaleFactor :  Screen.devicePixelRatio
    }


    Settings
    {
        id: wallpaperSettings
        category: "Wallpaper"
        property bool dim: true
        property bool fill: true
        property string defaultWallpaper: "qrc:/wallpapers/maui_shell_dev_bg.png"
        property string sourcePath: "file:///usr/share/wallpapers/Cask"
        property bool adaptiveColorScheme : true
    }

    function handleShellSurfaceCreated(shellSurface, toplevel)
    {
        console.log(shellSurface.windowType)

        let window = screen.zpaces.createXdgWindow(shellSurface, toplevel)

        screen.workspaces.currentIndex = screen.zpaces.addWindow(window, screen.workspaces.currentIndex)

        if(screen.formFactor !== Cask.Env.Desktop)
        {
            window.maximize()
        }

        window.activate()
    }
}
