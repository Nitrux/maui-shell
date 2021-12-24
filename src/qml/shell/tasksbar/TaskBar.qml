import QtQuick 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5

import org.kde.kirigami 2.7 as Kirigami
import org.mauikit.controls 1.3 as Maui
import org.maui.cask 1.0 as Cask
import Zpaces 1.0 as ZP
import QtQuick.Templates 2.15 as T

T.Control
{
    id: control

    Kirigami.Theme.inherit: false
    Kirigami.Theme.colorSet: Kirigami.Theme.Complementary

    implicitWidth: _section.implicitWidth + rightPadding + leftPadding
    implicitHeight: 64

    padding: Maui.Style.space.small
    spacing: Maui.Style.space.medium

    background: Rectangle
    {
        color: Kirigami.Theme.backgroundColor
        opacity: _dropArea.containsDrag ? 1 : 0.8
        radius: 10
    }

    contentItem: Flickable
    {
        contentWidth: _section.implicitWidth
        contentHeight: availableHeight
        clip: true

        Row
        {
            id: _section

            spacing: control.spacing

            DockItem
            {
                id: _launcherItem
                checked: _launcher.visible
                icon.name: "view-list-icons"
                onClicked: _launcher.toggle()
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

            Repeater
            {
                model: _zpaces.tasksModel

                AbstractButton
                {
                    focusPolicy: Qt.NoFocus
                    readonly property ZP.Task task : model.task
                    readonly property ZP.XdgWindow xdgWindow : task.window

                    implicitHeight: 48
                    implicitWidth: height
                    //                draggable: true
                    ToolTip.text: task.fileName
                    ToolTip.delay: 1000
                    ToolTip.timeout: 5000
                    ToolTip.visible: hovered

                    Menu
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
                            _menu.popup()
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
                            launchExec(task.executable)
                        }
                    }

                    contentItem: Item
                    {
                        Kirigami.Icon
                        {
                            source: task.iconName
                            height: 48
                            width: height
                            anchors.centerIn: parent
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



                //    onContentDropped:
                //    {
                //        console.log("Dropped things" , drop.urls)
                //        _tasksModel.append({icon: "vvave", title: "Apps Title", id : "appId", path: "desktopFIle"})
                //    }
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
