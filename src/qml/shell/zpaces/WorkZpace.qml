import QtQuick
import QtQml
import QtQuick.Controls

import QtWayland.Compositor

import org.mauikit.controls as Maui

import org.maui.cask as Cask

import Zpaces as ZP

ListView
{
    id: control
    property real overviewScale : 1
//    focusPolicy: Qt.NoFocus

    property bool overviewMode : false
    clip: false

    //    Binding on currentIndex
    //    {
    //        delayed: true
    //        value:  _appsOverview.currentIndex
    //        restoreMode: Binding.RestoreBinding
    //    }
    onCurrentIndexChanged:
    {
        control.lastPos = control.contentX
    }

    scale: control.overviewScale
    orientation: ListView.Horizontal
    interactive: overviewMode
    snapMode: ListView.SnapOneItem
    boundsBehavior: overviewMode ? Flickable.OvershootBounds : Flickable.StopAtBounds

    highlightFollowsCurrentItem: true
    highlightMoveDuration: Maui.Style.units.longDuration
    //    highlightResizeDuration: 0
    cacheBuffer: width * count
    //                    preferredHighlightEnd: width
    // 		highlight: Item {}
    //    highlightMoveVelocity: -1
    //    highlightResizeVelocity: -1


    property int lastPos: 0

//    Label
//    {
//        text: control.currentIndex + "/" + control.lastPos
//        color: "orange"
//        font.bold: true
//        font.pointSize: 22
//    }


    Connections
    {
        target: _zpaces.zpacesModel
        ignoreUnknownSignals: true
        function onZpaceAdded(index)
        {
            control.currentIndex = index
        }
    }

    function openOverview()
    {
        control.overviewScale = 0.8
        overviewMode = true
    }

    function closeOverview()
    {
        control.overviewScale = 1
        control.returnToBounds()
        overviewMode = false
        control.lastPos = control.currentIndex * control.width

    }

}
