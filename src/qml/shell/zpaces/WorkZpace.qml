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

    orientation: ListView.Horizontal
    interactive: overviewMode
    snapMode: overviewMode ? ListView.NoSnap : ListView.SnapOneItem
    boundsBehavior: Flickable.StopAtBounds

    highlightFollowsCurrentItem: true
    highlightMoveDuration: 0
    highlightResizeDuration: 0
    cacheBuffer: width * 4
    //                    preferredHighlightEnd: width
    // 		highlight: Item {}
    highlightMoveVelocity: -1
    highlightResizeVelocity: -1


    property int lastPos: 0

    Label
    {
        text: control.count + "/" + _zpaces.zpacesModel.count
        color: "orange"
        font.bold: true
        font.pointSize: 22
    }

    Connections
    {
        target: _zpaces.zpacesModel
        ignoreUnknownSignals: true
        function onZpaceAdded()
        {
            control.incrementCurrentIndex()
        }
    }


    function openOverview()
    {
         control.scale = 0.7
        overView = true
    }

    function closeOverview()
    {
        control.scale = 1
        overView = false
    }

}
