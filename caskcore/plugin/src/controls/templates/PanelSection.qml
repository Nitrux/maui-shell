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

    spacing: CaskShell.isMobile ? 0 : Maui.Style.space.medium
    hoverEnabled: true

    padding: 0
    topPadding: padding
    bottomPadding: padding
    leftPadding: padding
    rightPadding: padding

    implicitWidth: (contentItem ? contentItem.implicitWidth : 0) + leftPadding + rightPadding
    implicitHeight: (contentItem ? contentItem.implicitHeight : 0) + topPadding + bottomPadding

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

        // The popup is painted below the status bar, i.e. outside this section's
        // bounds. Qt Quick does not deliver input to children outside their
        // parent's bounds, so keep the popup in the full-window overlay and
        // position it there instead of inside the (small) panel section.
        parent: CaskShell.cask ? CaskShell.cask.overlayTopPanel : null
        z: 1

        finalYPos : availableGeometry.y
        availableGeometry: control.availableGeometry

        height: Math.min (availableGeometry.height, popup.implicitHeight)
        width: CaskShell.isMobile ? availableGeometry.width : Math.min(control.popWidth, availableGeometry.width)

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
            value: handler.active ? (handler.centroid.scenePosition.y - popup.height) : (popup.opened ? popup.finalYPos : availableGeometry.y)
        }

        x:
        {
            const baseX = parent ? control.mapToItem(parent, 0, 0).x : 0
            if (handler.active && CaskShell.formFactor === Cask.MauiMan.Desktop && !popup.opened)
                return handler.centroid.scenePosition.x - (width / 2)
            return baseX + setXAlignment(popup.alignment)
        }

        function setXAlignment(alignment)
        {
            switch(alignment)
            {
            case Qt.AlignLeft: return CaskShell.isMobile ? 0 : 0;
            case Qt.AlignCenter: return Math.round((control.width/2 ) - (popup.width/2));
            case Qt.AlignRight: return  0-(width - control.width)+(CaskShell.isMobile ? 0 : 0);
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
