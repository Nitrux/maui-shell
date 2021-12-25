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

Rectangle
{
    id: control
    Kirigami.Theme.inherit: false
    Kirigami.Theme.colorSet: Kirigami.Theme.Complementary

    property WorkZpace workzpace

    opacity: 0.9

    color: Kirigami.Theme.backgroundColor


    HoverHandler
    {
        id: _hadleHandler
        cursorShape: Qt.OpenHandCursor
    }

    Rectangle
    {
        x: ((parent.width/2) - width/2) - ((overviewHandler.centroid.position.x - overviewHandler.centroid.pressPosition.x) * -1)
        anchors.verticalCenter: parent.verticalCenter
        height: parent.height - 12
        width: Math.min(100, parent.width * 0.5)
        color: _hadleHandler.hovered ? Kirigami.Theme.highlightColor : Kirigami.Theme.textColor
        radius: 4
    }

//    Label
//    {
//        text: workzpace.lastPos + " / " + workzpace.contentX + " / " + workzpace.currentIndex
//    }

    Binding
    {
        when: overviewHandler.active && overviewHandler.centroid.position.y < -60
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
        value: target.lastPos + ((overviewHandler.centroid.position.x - overviewHandler.centroid.pressPosition.x) * -1)
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
                if(scaleValue < 0.7)
                {
                    _swipeView.openOverview()
                    //                    return;
                    //                    _appsOverview.initScale = control.workzpace.scale
                    //                    _appsOverview.contentX = control.workzpace.scale

                }else
                {
                    _swipeView.closeOverview()
                }

                workzpace.contentX += (overviewHandler.centroid.scenePosition.x - overviewHandler.centroid.scenePressPosition.x) * -1

                workzpace.returnToBounds()
                workzpace.currentIndex = workzpace.indexAt(workzpace.contentX, 0)

                //                overView = Math.abs(overviewHandler.centroid.position.y) > 100 && _appsOverview.count > 0
            }
        }
    }
}
