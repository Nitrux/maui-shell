import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import org.mauikit.controls as Maui

import org.maui.cask as Cask
import Zpaces as ZP

import QtQuick.Effects

import "../templates"

Control
{
    id: control

    implicitWidth: _section.implicitWidth + rightPadding + leftPadding
    implicitHeight: 64

    padding: Maui.Style.space.small
    spacing: padding

    property int radius: Maui.Style.radiusV

    Behavior on implicitWidth
    {
        NumberAnimation
        {
            duration: Maui.Style.units.shortDuration
            easing.type: Easing.InOutQuad
        }
    }

    background: Rectangle
    {
        id: _bg
        color: Maui.Theme.backgroundColor
        opacity: _dropArea.containsDrag ? 1 : 0.8
        radius: control.radius + control.padding

        Behavior on color
        {
            ColorAnimation
            {
                easing.type: Easing.InQuad
                duration: Maui.Style.units.shortDuration
            }
        }

        Loader
        {
            anchors.fill: parent
            active: false
            //            active: Maui.Style.enableEffects
            sourceComponent: Item
            {
                MultiEffect
                {
                    // opacity: 0.2
                    blurEnabled: true
                    blurMax: 64
                    saturation: -0.5
                    blur: 1.0
                    autoPaddingEnabled: true
                    source: ShaderEffectSource
                    {
                        live: true
                        sourceItem: CaskShell.cask ? CaskShell.cask.container : null
                        sourceRect: CaskShell.cask ? _bg.mapToItem(CaskShell.cask.container, Qt.rect(_bg.x, _bg.y, _bg.width, _bg.height)) : Qt.rect(0, 0, 0, 0)
                    }
                }

                Rectangle
                {
                    color: _bg.color
                    anchors.fill: parent
                    opacity: 0.6
                }

                // layer.enabled: true
                // layer.effect: MultiEffect
                // {
                //     maskEnabled: true
                //     maskThresholdMin: 0.5
                //     maskSpreadAtMin: 1.0
                //     maskSpreadAtMax: 0.0
                //     maskThresholdMax: 1.0
                //     maskSource: ShaderEffectSource
                //     {
                //         sourceItem:  Rectangle
                //         {
                //             width: _bg.width
                //             height: _bg.height
                //             radius: _bg.radius
                //         }
                //     }
                // }
            }
        }

        layer.enabled: CaskShell.formFactor === Cask.MauiMan.Desktop && Maui.Style.enableEffects
        layer.effect: MultiEffect
        {
            autoPaddingEnabled: true
            shadowEnabled: true
            shadowColor: "#000000"
        }
    }

    contentItem: Flickable
    {
        contentWidth: _section.implicitWidth
        contentHeight: availableHeight
        clip: true

        RowLayout
        {
            id: _section
            height: parent.height
            spacing: control.spacing

            DockItem
            {
                id: _launcherItem
                checked: CaskShell.dock && CaskShell.dock.launcher ? CaskShell.dock.launcher.visible : false
                icon.name: "view-list-icons"
                onClicked: { if(CaskShell.dock && CaskShell.dock.launcher) CaskShell.dock.launcher.toggle() }
            }

            Repeater
            {
                model: CaskShell.zpaces ? CaskShell.zpaces.tasksModel : null

                DockItem
                {
                    focusPolicy: Qt.NoFocus
                    readonly property ZP.Task task : model.task
                    readonly property ZP.XdgWindow xdgWindow : task.window
                    colorize: task.window ? task.window.toplevel.activated : false
                    checked: task.window ? task.window.isActive : false

                    icon.height: 32
                    icon.width: 32

                    text: task.fileName
                    icon.name: task.iconName

                    Maui.ContextualMenu
                    {
                        id: _menu

                        MenuItem
                        {
                            text: task.isPinned ? i18n("UnPin") : i18n("Pin")
                            onTriggered: task.isPinned = !task.isPinned
                        }

                        MenuItem
                        {
                            text: i18n("Maximize")
                        }

                        MenuSeparator{}
                        MenuItem
                        {
                            text: i18n("Close")
                            onTriggered: task.window.close()
                        }
                    }

                    MouseArea
                    {
                        anchors.fill: parent
                        acceptedButtons: Qt.RightButton
                        onClicked:
                        {
                            _menu.show()
                        }
                    }

                    onClicked:
                    {
                        if(xdgWindow)
                        {
                            CaskShell.workspaces.currentIndex = model.zpaceIndex


                            if(xdgWindow.toplevel.activated)
                            {
                                if(!xdgWindow.minimized)
                                {
                                    xdgWindow.minimize()
                                }else
                                {
                                    xdgWindow.unminimize()
                                }
                            }else
                            {
                                xdgWindow.unminimize()//also activates it
                            }
                        }else
                        {
                            CaskShell.appsDB.launchApp(task.path)
                        }
                    }

                    Rectangle
                    {
                        width: task.window ? (task.window.toplevel.activated ? parent.width  : height) : 0
                        anchors.horizontalCenter: parent.horizontalCenter
                        height: task.window ? (task.window.toplevel.activated ? 2  : 8) : 0
                        radius: height/2
                        anchors.bottom: parent.bottom
                        visible: task.window
                        color: Maui.Theme.highlightColor
                    }
                }
            }

            DockItem
            {
                icon.name: "view-file-columns"
                checked: CaskShell.appsOverview ? CaskShell.appsOverview.opened : false
                onClicked: { if(CaskShell.appsOverview) CaskShell.appsOverview.toggle() }
            }

            DockItem
            {
                icon.name: "list-add"

                visible: CaskShell.screen ? CaskShell.screen.overView : false
                onClicked: CaskShell.zpaces.insertZpace(0)
            }
        }

        WheelHandler
        {
            onWheel:
            {
                if(event.angleDelta.y > 0)
                {
                    CaskShell.zpaces.allSurfaces.activateNextWindow()
                }
            }
        }

    }

    DropArea
    {
        id: _dropArea
        anchors.fill: parent
        onDropped:
        {
            for(var url of drop.urls)
            {
                if(url.endsWith(".desktop"))
                {
                    CaskShell.zpaces.tasksModel.addTask(url, true)
                    if(CaskShell.dock && CaskShell.dock.launcher)
                        CaskShell.dock.launcher.close()
                }
            }
        }
    }
}
