import QtQuick
import QtQuick.Controls
import QtQuick.Window

import org.mauikit.controls as Maui

import org.maui.cask as Cask

import Zpaces as ZP

import QtQuick.Effects

import "../.."
import "../../chrome"

Pane
{
    id: control

    property ZP.Zpace zpace
    //    focusPolicy: Qt.NoFocus

    default property alias content: _content.data
    property alias container: _content

    property int radius: Maui.Style.radiusV
    property bool overviewMode : false
    property alias backgroundImage : _img.source
    property alias backgroundFillMode : _img.fillMode
    property alias backgroundVisible: _backgroundOverlay.visible

    readonly property rect availableGeometry : Qt.rect(0, 0, width, height)

    topPadding: CaskShell.cask ? CaskShell.cask.topPanel.height : 0
    bottomPadding: CaskShell.formFactor === Cask.MauiMan.Desktop && CaskShell.dock ? CaskShell.dock.height : 0
    leftPadding: 0
    rightPadding: 0

    Component.onCompleted:
    {
        control.zpace.control = control.container
    }

    Behavior on radius
    {
        NumberAnimation
        {
            duration: Maui.Style.units.longDuration
            easing.type: Easing.OutInQuad
        }
    }

    background: Item
    {
        id: _bg

        MouseArea
        {
            enabled: !overviewMode
            acceptedButtons: Qt.RightButton
            anchors.fill: parent
            propagateComposedEvents: false
            preventStealing: false
            onClicked:
            {
                _menu.show()
            }

            onPressAndHold:
            {
                _menu.show()
            }
        }

        Item
        {
            id : _bgContainer
            anchors.fill: parent

            Image
            {
                id: _img
                anchors.fill: parent
                anchors.margins: control.rise ? Maui.Style.space.large * 2 : 0
                asynchronous: true
                sourceSize.width: Screen.width
                sourceSize.height: Screen.height
                visible: false
            }
        }

        // ColorOverlay
        // {
        //     anchors.fill: _bgContainer
        //     source: _img
        //     color:  ?  : "transparent"

        //     layer.enabled: control.radius > 0 || control.rise
        //     layer.effect: OpacityMask
        //     {
        //         maskSource: Rectangle
        //         {
        //             width: _img.width
        //             height: _img.height
        //             radius: control.radius
        //         }
        //     }
        // }

        MultiEffect {
            id: _backgroundOverlay

            anchors.fill: _bgContainer
            source: _img
               colorization: Cask.MauiMan.background.dimWallpaper ? 1.0 : 0.0
               colorizationColor: "#80800000"
           }

        // DropShadow
        // {
        //     //            visible: control.rise
        //     transparentBorder: true
        //     anchors.fill: _bgContainer
        //     horizontalOffset: 0
        //     verticalOffset: 0
        //     radius: 8.0
        //     samples: 17
        //     color: Qt.rgba(0,0,0,0.9)
        //     source: _bgContainer
        // }
    }

    contentItem : Item
    {
        id: _container
        data: Repeater
        {
            id: _repeater
            model: zpace ? zpace.windows : null

            onItemRemoved:
            {
                control.focusTopWindow(1)
            }

            delegate: Chrome
            {
                id: _chromeDelegate
                parent: _content

                overviewMode: control.overviewMode
                shellSurface: model.window.shellSurface
                window: model.window
                zpaceItem: control
                scale: CaskShell.isMobile ? 1 : CaskShell.workspaces.scale


                moveItem: Item
                {
                    objectName: "moveItem"
                    property bool moving: false

                    parent: _container

                    x: 0
                    y: 0

                    height: _chromeDelegate.height
                    width: _chromeDelegate.width
                }
            }
        }

        Item
        {
            id: _content
            anchors.fill: parent
            objectName: control.objectName
        }

        TapHandler
        {
            enabled: overviewMode

            //            anchors.fill: parent
            //            propagateComposedEvents: true
            //            preventStealing: false
            onTapped:
            {

                CaskShell.workspaces.currentIndex = index
                CaskShell.workspaces.closeOverview()
            }
        }

        //        Label
        //        {
        //            anchors.centerIn: parent
        //            color: "pink"
        //            font.bold: true
        //            text:
        //            {
        //                var res = ""
        //                for(var i = 0; i< _content.children.length; i++)
        //                {
        //                    res+= _content.children[i]+ " / " + _content.children[i].objectName+"\n"
        //                }

        //                return res
        //            }
        //        }

        Maui.ContextualMenu
        {
            id: _menu

            MenuItem
            {
                text: i18n("Wallpaper")
                icon.name: "insert-image"
                onTriggered:  Cask.MauiMan.invokeManager("background")
            }

            MenuItem
            {
                text: i18n("Widgets")
                icon.name: "draw-cuboid"
            }

            MenuSeparator {}

            MenuItem
            {
                text: i18n("About")
                icon.name: "documentinfo"
                onTriggered: Cask.MauiMan.invokeManager("about")
            }
        }
    }

    function focusWindow(index)
    {
        // Called after a window is removed, so the requested index can point
        // past the end (or the child may not be a window yet). Bail out
        // instead of dereferencing undefined.
        if (index < 0 || index >= _content.children.length)
            return

        const item = _content.children[index]
        if (!item || !item.window)
            return

        item.window.activate()
    }

    function focusTopWindow(index = 0)
    {
        focusWindow(_content.children.length - 1 - index)
    }
}
