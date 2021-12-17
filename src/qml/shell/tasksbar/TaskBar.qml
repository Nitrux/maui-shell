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

    Layout.fillWidth: false
    Layout.alignment: Qt.AlignCenter
    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)

    Layout.margins: Maui.Style.space.medium
    implicitHeight: 64
    padding: Maui.Style.space.small

    //    property alias launcher : _launcherItem



    background: Rectangle
    {
        color: Kirigami.Theme.backgroundColor
        opacity: _dropArea.containsDrag ? 1 : 0.8
        radius: 10
    }

    contentItem: Row
    {
        id: _section

        spacing: Maui.Style.space.medium


        LauncherPanelItem
        {
            id: _launcherItem
            height: parent.height
            onClicked: _launcher.toggle()
        }

        Cask.PanelItem
        {
            iconSize: 48
            height: parent.height
            leftPadding: 0
            rightPadding: 0
            visible: !isMobile
            background: Rectangle
            {
                color: Qt.darker(Kirigami.Theme.backgroundColor)
                radius: 10
                opacity: 0.7
            }

            Item
            {
                Layout.preferredHeight: 48
                Layout.preferredWidth: height


                Kirigami.Icon
                {
                    source: "view-file-columns"
                    color: Kirigami.Theme.textColor
                    height: 22
                    width: height
                    anchors.centerIn: parent
                }
            }

            //            visible: !isMobile
            checked: _appsOverview.opened
            onClicked: _appsOverview.toggle()
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
