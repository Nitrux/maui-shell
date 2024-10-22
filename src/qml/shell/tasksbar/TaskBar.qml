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
                        sourceItem: cask.container
                        sourceRect: _bg.mapToItem(cask.container, Qt.rect(_bg.x, _bg.y, _bg.width, _bg.height))
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

        layer.enabled: win.formFactor === Cask.MauiMan.Desktop && Maui.Style.enableEffects
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
                checked: _launcher.visible
                icon.name: "view-list-icons"
                onClicked: _launcher.toggle()
            }

            Repeater
            {
                model: _zpaces.tasksModel

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
                            _swipeView.currentIndex = model.zpaceIndex

                            console.log("MINIMIZED?" , xdgWindow.minimized)

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
                            _appsDB.launchApp(task.path)
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
                checked: _appsOverview.opened
                onClicked: _appsOverview.toggle()
            }

            DockItem
            {
                icon.name: "list-add"

                visible: overView
                onClicked: _zpaces.insertZpace(0)
            }
        }

        WheelHandler
        {
            onWheel:
            {
                console.log("WHell on dock")
                if(event.angleDelta.y > 0)
                {
                    _zpaces.allSurfaces.activateNextWindow()
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
                    console.log("Dropped things" , url)
                    zpaces.tasksModel.addTask(url, true)
                    launcher.close()
                }
            }
        }
    }
}
