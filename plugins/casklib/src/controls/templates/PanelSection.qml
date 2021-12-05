import QtQuick 2.15
import QtQml 2.14
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.15
import QtGraphicalEffects 1.0

import org.kde.kirigami 2.7 as Kirigami
import org.mauikit.controls 1.2 as Maui
import org.maui.cask 1.0 as Cask

import QtQuick.Templates 2.15 as T

T.Control
{
    id: control

    default property alias content : _content.data

    property alias cards : popup.contentChildren
    property alias popup : popup

    property int popWidth : Math.max(100, control.width)
    property alias popHeight : popup.height
    property alias alignment: popup.alignment

    spacing:  Maui.Style.space.medium

    property int position
    property int currentCard : -1

    padding: 0
    topPadding: padding
    bottomPadding: padding
    leftPadding: padding
    rightPadding: padding

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)

    implicitHeight: _content.implicitHeight + topPadding + bottomPadding


    Behavior on implicitWidth
    {
        NumberAnimation
        {
            duration: Kirigami.Units.longDuration
            easing.type: Easing.OutInQuad
        }
    }

    PanelPopup
    {
        id: popup
        property int alignment: Qt.AlignCenter
        z: _content.z -1
        position: control.position
        Label
        {
            color: "orange"
            text:  handler.centroid.position.y + " / " + handler2.centroid.scenePosition.y
            anchors.bottom: parent.bottom
        }

        onOverlayClicked: control.close()
        //        onActiveFocusChanged:
        //        {
        //            if(!activeFocus)
        //                control.close()
        //        }

        visible: handler.active || opened
        opacity: control.position === ToolBar.Footer ? (y/finalYPos)  : Math.abs((y+height)/(0-(height)))



        Binding on y
        {
            //            when: !handler.active
            value: handler.active ? (handler.centroid.position.y -(control.position === ToolBar.Footer ? 0 : popup.height) ) : (popup.opened ? popup.finalYPos : control.position === ToolBar.Footer ? 0 : 0-popup.height)
            restoreMode: Binding.RestoreBindingOrValue
        }

        //        Behavior on y
        //        {
        //            NumberAnimation
        //            {
        //                duration: Kirigami.Units.longDuration*3
        //                easing.type: Easing.OutInQuad
        //            }
        //        }

        //        Behavior on x
        //        {
        //            NumberAnimation
        //            {
        //                duration: Kirigami.Units.longDuration*10
        //                easing.type: Easing.InOutQuad
        //            }
        //        }

        x: handler.active && win.formFactor === Cask.Env.Desktop && !popup.opened ? (handler.centroid.pressPosition.x - (width/2)) : setXAlignment(popup.alignment)

        function setXAlignment(alignment)
        {
            switch(alignment)
            {
            case Qt.AlignLeft: return 0;
            case Qt.AlignCenter: return Math.round((control.width/2 ) - (popup.width/2));
            case Qt.AlignRight: return 0-(width - control.width);
            default: return 0;
            }

        }

        readonly property int finalYPos : control.position === ToolBar.Footer ? 0 - (popup.height) : control.height + Maui.Style.space.medium

        height: Math.min (_cask.avaliableHeight, popup.implicitHeight) + _cask.topPanel.implicitHeight
        width: isMobile ? _cask.width : Math.min(control.popWidth, _cask.width)

        function close()
        {
            popup.opened = false
            popup.closed()
        }

        function open()
        {
            popup.opened = true
            popup.y= popup.opened ? popup.finalYPos : control.position === ToolBar.Footer ? 0 : 0-popup.height
            popup.forceActiveFocus()
            //            popup.opened()
        }

        DragHandler
        {
            id: handler2
            dragThreshold: 100
            enabled: popup.opened && Maui.Handy.isTouch
            target: popup
            yAxis.minimum: control.position === ToolBar.Footer  ? popup.finalYPos : undefined
            yAxis.maximum: control.position === ToolBar.Footer  ? undefined : popup.finalYPos

            xAxis.enabled : false
            grabPermissions: PointerHandler.CanTakeOverFromItems | PointerHandler.CanTakeOverFromHandlersOfDifferentType | PointerHandler.ApprovesTakeOverByAnything
            onActiveChanged:
            {
                const condition = control.position === ToolBar.Footer ? (handler2.centroid.scenePosition.y - handler2.centroid.scenePressPosition.y > 200) : (handler2.centroid.scenePosition.y - handler2.centroid.scenePressPosition.y < -200)
                if(!active && condition)
                {
                    control.close()
                }else
                {
                    popup.open()
                }
            }
        }
    }

    DragHandler
    {
        id: handler
        dragThreshold: control.position === ToolBar.Footer ? 64 : 20
        enabled: !popup.opened && Maui.Handy.isTouch
        target: null
        yAxis.minimum: control.position === ToolBar.Footer ? popup.finalYPos - 10 : 0
        yAxis.maximum: control.position === ToolBar.Footer  ? 0 : popup.finalYPos + 10
        xAxis.enabled : false
        grabPermissions: PointerHandler.CanTakeOverFromAnything
        onActiveChanged:
        {
            console.log(Math.abs(handler.centroid.scenePressPosition.y -handler.centroid.scenePosition.y))

            if(!active && Math.abs(handler.centroid.scenePressPosition.y -handler.centroid.scenePosition.y) > 60)
            {
                popup.open()
            }else
            {
                control.close()
            }
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
    }

    function isPanelItem(obj)
    {
        return obj instanceof PanelCard
    }

    Component.onCompleted:
    {
        for(var k in _content.children)
        {
            const obj = _content.children[k]
            if(obj.card && isPanelItem(obj.card))
            {
                obj.card.index = popup.count
                obj.card.visible = Qt.binding(function(){return (control.currentCard >= 0 ? control.currentCard === obj.card.index : true) })
                obj.card.parent = popup
                popup.container.insertItem(popup.count, obj.card)
            }
        }
    }


    function close()
    {
        console.log("Close it")
        control.currentCard = -1
        popup.close()
    }

    function open(index)
    {
        console.log("Pop it up", index)
        if(index >= 0 && index  <= popup.count)
        {
            control.currentCard = index
        }else
        {
            control.currentCard = -1
        }

        popup.open()
    }


    Component.onDestruction:
    {
        console.log("DESTROY PANEL SECTION")
    }

}
