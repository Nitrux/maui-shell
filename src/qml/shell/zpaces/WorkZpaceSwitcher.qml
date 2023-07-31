import QtQuick
import QtQml
import QtQuick.Controls
import QtQuick.Layouts

import QtQuick.Window

import Qt5Compat.GraphicalEffects

import QtWayland.Compositor
import org.mauikit.controls as Maui
import org.maui.cask as Cask

import Zpaces as ZP

Rectangle
{
    id: control

    property WorkZpace workzpace

    opacity: 0.9

    color: Maui.Theme.backgroundColor


    HoverHandler
    {
        id: _hadleHandler
        cursorShape: Qt.OpenHandCursor
    }

    Rectangle
    {
        x: overviewHandler.active? overviewHandler.centroid.scenePosition.x - (width/2) : ((control.width/2) - (width/2))
        anchors.verticalCenter: parent.verticalCenter
        height: parent.height - 12
        width: Math.min(100, parent.width * 0.5)
        color: _hadleHandler.hovered ? Maui.Theme.highlightColor : Maui.Theme.textColor
        radius: Maui.Style.radiusV
    }

//    Label
//    {
//        text: workzpace.lastPos + " / " + workzpace.contentX + " / " + workzpace.currentIndex
//    }

    property bool scaleEnabled : overviewHandler.centroid.position.y < -60

    Binding
    {
        when: overviewHandler.active && scaleEnabled
        target: control.workzpace
        property: "overviewScale"
        value: overviewHandler.scaleValue
    }

    Binding
    {
        target: control.workzpace
        property: "contentX"
        when: overviewHandler.active
        delayed: true
        value: control.workzpace.lastPos + ((overviewHandler.centroid.scenePosition.x - overviewHandler.centroid.scenePressPosition.x) * -1)
        restoreMode: Binding.RestoreBinding
    }


    DragHandler
    {
        id: overviewHandler
        target: null
        property real scaleValue : Math.max(0.6, overviewHandler.centroid.scenePosition.y / overviewHandler.centroid.scenePressPosition.y)
        onActiveChanged:
        {
            if(!active)
            {
                if(scaleValue > 0)
                {
                    if(scaleValue < 0.7)
                    {
                        _swipeView.openOverview()
                        //                    _appsOverview.initScale = control.workzpace.scale
                        //                    _appsOverview.contentX = control.workzpace.scale

                    }else
                    {
                        _swipeView.closeOverview()

                        workzpace.currentIndex = workzpace.indexAt(workzpace.contentX, workzpace.contentY)

                    }

                    return;
                }



//                workzpace.returnToBounds()
                workzpace.currentIndex = workzpace.indexAt(workzpace.contentX, workzpace.contentY)
                //

//                workzpace.positionViewAtIndex(workzpace.currentIndex, ListView.SnapPosition )
            }
        }
    }
}
