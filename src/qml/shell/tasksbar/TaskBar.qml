import QtQuick 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5

import org.kde.kirigami 2.7 as Kirigami
import org.mauikit.controls 1.3 as Maui

import org.maui.cask 1.0 as Cask
import Zpaces 1.0 as ZP

import QtQuick.Templates 2.15 as T
import QtGraphicalEffects 1.0

import "../templates"

T.Control
{
    id: control

    implicitWidth: _section.implicitWidth + rightPadding + leftPadding
    implicitHeight: 64

    padding: Maui.Style.space.small
    spacing: padding

    property int radius: 10

    Behavior on implicitWidth
    {
        NumberAnimation
        {
            duration: Kirigami.Units.shortDuration
            easing.type: Easing.InOutQuad
        }
    }

    background: Rectangle
    {
        color: Kirigami.Theme.backgroundColor
        opacity: _dropArea.containsDrag ? 1 : 0.8
        radius: control.radius

        Behavior on color
        {
            ColorAnimation
            {
                easing.type: Easing.InQuad
                duration: Kirigami.Units.longDuration
            }
        }

        layer.enabled: win.formFactor === Cask.Env.Desktop
        layer.effect: DropShadow
        {
            horizontalOffset: 0
            verticalOffset: 0
            samples: 10
            color: Qt.rgba(0,0,0,0.5)
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
                        color: Kirigami.Theme.highlightColor
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
