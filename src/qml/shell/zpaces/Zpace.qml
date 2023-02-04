import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Templates 2.15 as T

import org.mauikit.controls 1.3 as Maui

import org.maui.cask 1.0 as Cask

import Zpaces 1.0 as ZP

import QtGraphicalEffects 1.0

import "../.."
import "../../chrome"

T.Pane
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

    topPadding: cask.topPanel.height
    bottomPadding: formFactor === Cask.MauiMan.Desktop ? dock.height : 0
    leftPadding: 0
    rightPadding: 0

    //    clip: false

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

        ColorOverlay
        {
            id: _backgroundOverlay
            anchors.fill: _bgContainer
            source: _img
            color: Cask.MauiMan.background.dimWallpaper ? "#80800000" : "transparent"

            layer.enabled: control.radius > 0 || control.rise
            layer.effect: OpacityMask
            {
                maskSource: Rectangle
                {
                    width: _img.width
                    height: _img.height
                    radius: control.radius
                }
            }
        }

        DropShadow
        {
            //            visible: control.rise
            transparentBorder: true
            anchors.fill: _bgContainer
            horizontalOffset: 0
            verticalOffset: 0
            radius: 8.0
            samples: 17
            color: Qt.rgba(0,0,0,0.9)
            source: _bgContainer
        }
    }

    contentItem : Item
    {
        data: Repeater
        {
            id: _repeater
            model: zpace.windows

            onItemRemoved:
            {
                control.focusTopWindow(1)
            }

            delegate: Chrome
            {
                parent: _content
                id: _chromeDelegate
                overviewMode: control.overviewMode
                shellSurface: model.window.shellSurface
                window: model.window
                scale: isMobile ? 1 : _swipeView.scale
                moveItem: Item
                {
                    objectName: "moveItem"
                    property bool moving: false
                    parent: control
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

                _swipeView.currentIndex = index
                _swipeView.closeOverview()
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
        _content.children[index].window.activate()
    }

    function focusTopWindow(index = 0)
    {
        focusWindow(_content.children.length - 1 - index)
    }
}
