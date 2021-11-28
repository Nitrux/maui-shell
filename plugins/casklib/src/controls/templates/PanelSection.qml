import QtQuick 2.15
import QtQml 2.14
import QtQuick.Window 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.13
import QtGraphicalEffects 1.0

import org.kde.kirigami 2.7 as Kirigami
import org.mauikit.controls 1.2 as Maui
import org.maui.cask 1.0 as Cask

Control
{
    id: control
    default property alias content : _content.data

    property alias cards : popup.contentChildren
    property alias popup : popup
    property int popWidth : Math.max(100, control.width)
    property alias popHeight : popup.height
    property alias alignment: popup.alignment
    property color backgroundColor: Kirigami.Theme.backgroundColor
    property int radius : 0
    spacing: Maui.Style.space.medium


    property int position

    property int margins : 0


    property int currentCard : -1

    Layout.minimumWidth: 0
    Layout.preferredWidth: implicitWidth
    Layout.margins: margins

    Layout.fillWidth: false
    Layout.preferredHeight: implicitHeight

    implicitWidth: _content.implicitWidth + Maui.Style.space.big

    implicitHeight: _content.implicitHeight

    background: Rectangle
    {
        id: _rec
        radius: control.radius
        opacity: 0.8
        color: control.backgroundColor

        Behavior on radius
        {
            NumberAnimation
            {
                duration: Kirigami.Units.longDuration
                easing.type: Easing.InOutQuad
            }
        }
    }

    Behavior on margins
    {
        NumberAnimation
        {
            duration: Kirigami.Units.longDuration
            easing.type: Easing.InOutQuad
        }
    }

    PanelPopup
    {
        id: popup
        property int alignment: Qt.AlignCenter
        z: _content.z + 1
position: control.position
        Label
        {
            color: "orange"
            text:  handler.centroid.pressPosition.x + " / " + handler.centroid.scenePressPosition.y
            anchors.bottom: parent.bottom
        }

        onOverlayClicked: control.close()

        visible: handler.active || opened
        opacity: control.position === ToolBar.Footer ? (y/finalYPos)  : Math.abs((y+height)/(0-(height)))

        Binding on y
        {
            when: !handler.active
            value: popup.opened ? popup.finalYPos : control.position === ToolBar.Footer ? 0 : 0-popup.height
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

        height: Math.min (_cask.avaliableHeight-control.height, popup.implicitHeight)
        width: Math.min(control.popWidth, _cask.avaliableWidth)

        function close()
        {
            popup.opened = false
        }

        function open()
        {
            popup.opened = true
            popup.y= popup.opened ? popup.finalYPos : control.position === ToolBar.Footer ? 0 : 0-popup.height
        }

        DragHandler
        {
            id: handler2
            dragThreshold: 100
            enabled: popup.opened
            target: popup
            yAxis.minimum: control.position === ToolBar.Footer  ? popup.finalYPos : undefined
            yAxis.maximum: control.position === ToolBar.Footer  ? undefined : popup.finalYPos

            xAxis.enabled : false
            grabPermissions: PointerHandler.CanTakeOverFromItems | PointerHandler.CanTakeOverFromHandlersOfDifferentType | PointerHandler.ApprovesTakeOverByAnything
            onActiveChanged:
            {
                const condition = control.position === ToolBar.Footer ? handler2.centroid.scenePosition.y - handler2.centroid.scenePressPosition.y > 200 : handler2.centroid.scenePressPosition.y - handler2.centroid.scenePosition.y > -200
                if(!active && condition)
                {
                    popup.close()
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
        dragThreshold: 100
        enabled: !popup.opened
        target: popup
        yAxis.minimum: control.position === ToolBar.Footer  ? popup.finalYPos - 10 : undefined
        yAxis.maximum: control.position === ToolBar.Footer  ? undefined : popup.finalYPos + 10
        xAxis.enabled : false
        grabPermissions: PointerHandler.CanTakeOverFromAnything
        onActiveChanged:
        {
            console.log(Math.abs(handler.centroid.scenePressPosition.y -handler.centroid.scenePosition.y))
            if(!active && Math.abs(handler.centroid.scenePressPosition.y -handler.centroid.scenePosition.y) > 200)
            {
                popup.open()
            }else
            {
                popup.close()
            }
        }
    }

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
            if(obj.card)
            {
                obj.card.index = popup.count
                obj.card.visible = Qt.binding(function(){return (control.currentCard >= 0 ? control.currentCard === obj.card.index : true) })
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

}
