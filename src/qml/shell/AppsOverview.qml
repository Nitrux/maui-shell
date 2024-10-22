import QtQuick
import QtQml
import QtQuick.Controls
import QtQuick.Window
import QtQuick.Layouts

import QtQuick.Effects

import QtWayland.Compositor

import org.maui.cask as Cask
import org.mauikit.controls as Maui

import Zpaces as ZP

Maui.Page
{
    id: control

    property bool opened : false
    //    property alias currentIndex : _overviewList.currentIndex
    //    property alias count : _overviewList.count
    property real initScale : 0.8
    property alias contentX : _flickable.contentX
    headBar.visible: false

    background: MultiEffect
    {
        // opacity: 0.2
        blurEnabled: true
        blurMax: 64
        saturation: -0.5
        blur: 1.0
        autoPaddingEnabled: true
        source:  _cask.superOverlay


        Rectangle
        {
            anchors.fill: parent
            color: Maui.Theme.backgroundColor
            opacity: 0.8
        }
    }

    MouseArea
    {
        anchors.fill: parent
        propagateComposedEvents: true
        preventStealing: false
        onClicked:
        {
            control.close()
            showDesktop = true
        }
    }

    //    footBar.opacity: _overviewList.opacity
    footBar.visible: true
    footBar.middleContent: Row
    {
        Layout.alignment: Qt.AlignCenter
        spacing: footBar.spacing
        Button
        {
            text: qsTr("Clear all")
            onClicked:
            {
                overView = false
                //                _listSurfaces.clear()
            }
        }

        Button
        {
            text: qsTr("Show Desktop")
            onClicked:
            {
                control.close()
                showDesktop = true
            }
        }

        Button
        {
            text: qsTr("Settings")
        }
    }

    MouseArea
    {
        anchors.fill : parent
        onClicked: control.close()
    }

    Flickable
    {
        id: _flickable
        anchors.fill: parent
        contentWidth: availableWidth
        contentHeight: _overviewList.implicitHeight
        contentX: _swipeView.contentX
        //        scale: control.initScale
        interactive: false

        boundsBehavior: Flickable.StopAtBounds
        boundsMovement :Flickable.StopAtBounds

        Flow
        {
            id: _overviewList
            //        opacity: overView ? 1 : (1* Math.min(200, overviewHandler.centroid.position.y * -1)) / 200
            //        highlightFollowsCurrentItem: true
            //        Binding on currentIndex
            //        {
            //            delayed: true
            //            value:  _swipeView.currentIndex
            //            restoreMode: Binding.RestoreBinding
            //        }

            height: parent.height
            width: parent.width
            //        anchors.verticalCenter: parent.verticalCenter

            spacing: Maui.Style.space.big

            //        onCountChanged:
            //        {
            //            if(_overviewList.model.count <= 0)
            //                overView = false
            //        }
            Repeater
            {
                model: _zpaces.allSurfaces
                delegate: ItemDelegate
                {
                    id: _itemDelegate


                    property ZP.XdgWindow xdgWindow :  model.window
                    height: _layout.implicitHeight
                    width: _layout.implicitWidth

                    opacity: (y * -1) > 50 ? 20 / (y * -1): 1
                    background: null
                    onClicked:
                    {
                        control.close()
                        _swipeView.currentIndex = _zpaces.indexOfZpace(xdgWindow)
                        if(!xdgWindow.toplevel.activated)
                        {
                            xdgWindow.unminimize()//also activates it
                        }
                    }

                    clip: false
                    contentItem: Column
                    {
                        id: _layout

                        Maui.ListItemTemplate
                        {
                            width: parent.width
                            height: Maui.Style.rowHeight
                            iconSource: xdgWindow.iconName
                            iconSizeHint: Maui.Style.iconSizes.medium
                            label1.text: xdgWindow.appName



                            Maui.CloseButton
                            {
                                onClicked: xdgWindow.close()
                            }
                        }

                        Item
                        {
                            height: _surface.surface.sourceGeometry.height * 0.5
                            width: _surface.surface.sourceGeometry.width * 0.5
                            //                    scale: 0.7
                            id: _thumbnail

                            WaylandQuickItem
                            {
                                id: _surface
                                surface: xdgWindow.shellSurface.surface
                                touchEventsEnabled: false
                                focusOnClick: false
                                enabled: false
//                                sizeFollowsSurface: false
                                smooth: true
                                anchors.fill: parent

                                layer.enabled: true
                                layer.effect: MultiEffect
                                {
                                    maskEnabled: true
                                    maskThresholdMin: 0.5
                                    maskSpreadAtMin: 1.0
                                    maskSpreadAtMax: 0.0
                                    maskThresholdMax: 1.0
                                    maskSource: ShaderEffectSource
                                    {
                                        sourceItem: Rectangle
                                        {
                                            width: _thumbnail.width
                                            height: _thumbnail.height

                                            radius: Maui.Style.radiusV

                                        }
                                    }
                                }
                            }

                            Rectangle
                            {
                                anchors.fill: parent
                                //         anchors.margins: Maui.Style.space.small
                                radius: Maui.Style.radiusV
                                color: "transparent"
                                border.color: Qt.darker(Maui.Theme.backgroundColor, 2.7)
                                opacity: 0.5

                                Rectangle
                                {
                                    anchors.fill: parent
                                    anchors.margins: 1
                                    color: "transparent"
                                    radius: parent.radius - 0.5
                                    border.color: Qt.lighter(Maui.Theme.backgroundColor, 2)
                                    opacity: 0.7
                                }
                            }

                            layer.enabled:true
                            layer.effect: MultiEffect
                            {
                                autoPaddingEnabled: true
                                shadowEnabled: true
                                shadowColor: "#000000"
                            }
                        }
                    }

                    DragHandler
                    {
                        target: parent
                        xAxis.enabled: false
                        onActiveChanged:
                        {
                            if(!active && (target.y * -1) > 100)
                                xdgWindow.close()
                            else target.y = 0
                        }
                    }

                }
            }
        }
    }

    function open()
    {
        control.opened = true
    }

    function close()
    {
        control.opened = false
    }

    function toggle()
    {
        if(control.opened)
        {
            control.close()
        }else
        {
            control.open()
        }
    }
}

