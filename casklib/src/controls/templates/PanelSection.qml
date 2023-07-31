import QtQuick
import QtQml
import QtQuick.Layouts
import QtQuick.Controls

import org.mauikit.controls as Maui
import org.maui.cask as Cask

Control
{
    id: control

    default property alias content : _content.data

    property alias cards : popup.contentChildren
    property alias popup : popup

    property int popWidth : 340
    property alias popHeight : popup.height
    property alias alignment: popup.alignment
    property alias visibleChildren: _content.visibleChildren

    property rect availableGeometry
    property int currentCard : -1

    spacing: isMobile ? 0 : Maui.Style.space.medium
    hoverEnabled: true

    padding: 0
    topPadding: padding
    bottomPadding: padding
    leftPadding: padding
    rightPadding: padding

    implicitWidth: implicitContentWidth + leftPadding + rightPadding
    implicitHeight: implicitContentHeight + topPadding + bottomPadding

    Behavior on implicitWidth
    {
        NumberAnimation
        {
            duration: Maui.Style.units.longDuration
            easing.type: Easing.OutInQuad
        }
    }

    //    Label
    //    {
    //        text: control.implicitHeight
    //    }

    contentItem: Row
    {
        id: _content
        spacing: control.spacing
        clip: true
    }

    DragHandler
    {
        id: handler
        dragThreshold: 20
        enabled: !popup.opened /*&& Maui.Handy.isTouch*/
        target: null

//        yAxis.minimum: 0
//        yAxis.maximum: popup.finalYPos + 10
        xAxis.enabled : false
        yAxis.enabled: true
        grabPermissions: PointerHandler.CanTakeOverFromAnything
        onActiveChanged:
        {
            console.log(Math.abs(handler.centroid.scenePressPosition.y - handler.centroid.scenePosition.y))
            if(active)
            {
                control.fillPopup()
            }

            const scenePressPos = handler.centroid.scenePressPosition.y
            const scenePos = handler.centroid.scenePosition.y

            if(!active && Math.abs(scenePressPos -scenePos) > 60)
            {
                popup.open()
            }else
            {
                popup.close()
            }
        }
    }

    PanelPopup
    {
        id: popup
        z: _content.z - 6
        finalYPos : availableGeometry.y

        height: Math.min (availableGeometry.height, popup.implicitHeight)
        width: isMobile ? availableGeometry.width : Math.min(control.popWidth, availableGeometry.width)

        //        Label
        //        {
        //            color: "orange"
        //            text:  handler.centroid.position.y + " / " + handler2.centroid.scenePosition.y
        //            anchors.bottom: parent.bottom
        //        }

        onOverlayClicked:
        {
            clear()
            close()
        }

        visible: handler.active || opened
        opacity: Math.abs((y+height)/(0-(height)))

        Binding on y
        {
            delayed: true
            restoreMode: Binding.RestoreBindingOrValue
            value: handler.active ? (handler.centroid.position.y -popup.height) : (popup.opened ? popup.finalYPos : availableGeometry.y)
        }

        x: handler.active && win.formFactor === Cask.MauiMan.Desktop && !popup.opened ? (handler.centroid.pressPosition.x - (width/2)) : setXAlignment(popup.alignment)

        function setXAlignment(alignment)
        {
            switch(alignment)
            {
            case Qt.AlignLeft: return isMobile ? 0 : 0;
            case Qt.AlignCenter: return Math.round((control.width/2 ) - (popup.width/2));
            case Qt.AlignRight: return  0-(width - control.width)+(isMobile ? 0 : 0);
            default: return 0;
            }
        }
    }

    data: WheelHandler
    {
        //        orientation: Qt.Vertical
        target: null
        enabled: true
        grabPermissions: PointerHandler.CanTakeOverFromAnything

        //        property: "y"
        //        rotationScale: 15
        //        acceptedDevices: PointerDevice.Mouse | PointerDevice.TouchPad
        onActiveChanged:
        {
            console.log("WHEEL HANMDLER ACTIVE", rotation)

            if(active && !popup.opened)
            {
                fillPopup()
            }

            if(!active)
            {
                if(rotation > 15)
                {
                    popup.slideUp()
                }
                else if(rotation < -60)
                {
                    if(popup.opened)
                    {
                        return
                    }

                    popup.slideDown()
                }
                rotation = 0
            }
        }
    }

    function isPanelCard(obj)
    {
        return obj instanceof PanelCard
    }

    function close(card)
    {
        if(card && isPanelCard(card))
        {
            console.log("Close it", card.index)

            var card_ = popup.takeItem(card.index)
            console.log("REMOVING CARD", card.parent)
            if(popup.count === 0)
            {
                popup.close()
            }
        }
    }

    function open(card)
    {
        if(card)
        {
            console.log("Pop it up", card)

            popup.addCard(card)
            console.log("INSERTING CARD", card.parent)
            popup.open()
        }
    }

    function fillPopup()
    {
        for(var k in _content.children)
        {
            const obj = _content.children[k]
            if(obj.card)
            {
                console.log("INSERTING CARD", obj.card)
                popup.addCard(obj.card)
            }
        }
    }

    Component.onDestruction:
    {
        console.log("DESTROY PANEL SECTION")
    }

}
