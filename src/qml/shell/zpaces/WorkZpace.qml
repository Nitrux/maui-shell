import QtQuick 2.15
import QtQml 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.3

import QtQuick.Window 2.15

import QtGraphicalEffects 1.0

import QtWayland.Compositor 1.0

import org.mauikit.controls 1.2 as Maui
import org.kde.kirigami 2.8 as Kirigami
import org.maui.cask 1.0 as Cask

import Zpaces 1.0 as ZP

ListView
{
    id: control
    property real overviewScale : 1

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
    highlightMoveDuration: Kirigami.Units.longDuration
    //    highlightResizeDuration: 0
    cacheBuffer: width * count
    //                    preferredHighlightEnd: width
    // 		highlight: Item {}
    //    highlightMoveVelocity: -1
    //    highlightResizeVelocity: -1


    property int lastPos: 0

    Label
    {
        text: control.currentIndex + "/" + control.lastPos
        color: "orange"
        font.bold: true
        font.pointSize: 22
    }


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
