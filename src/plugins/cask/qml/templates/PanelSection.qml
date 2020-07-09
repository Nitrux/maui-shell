import QtQuick 2.13
import QtQml 2.14
import QtQuick.Window 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.13
import QtGraphicalEffects 1.0

import org.kde.kirigami 2.7 as Kirigami
import org.kde.mauikit 1.2 as Maui

Item
{
    id: control
    default property alias content : _content.content
    property alias cards : popup.contentChildren
    property int popWidth : Math.min(_cask.avaliableWidth, control.width)
    property alias popHeight : popup.height

    property alias backgroundColor: _rec.color
    property alias radius : _rec.radius
    property alias spacing: _content.spacing
    property alias leftContent: _content.leftContent
    property alias middleContent: _content.middleContent
    property alias rightContent: _content.rightContent
    property alias position :_content.position

    property bool collapsed : false

    property alias preferredHeight: _content.preferredHeight
    property int margins : Maui.Style.space.medium

    property int currentCard : -1

    Layout.minimumWidth: 0
    Layout.preferredWidth: implicitWidth
    Layout.margins: margins

    Layout.fillWidth: false
    Layout.preferredHeight: preferredHeight

    implicitWidth: collapsed ? 0 : _content.implicitWidth + Maui.Style.space.big

    signal contentDropped(var drop)

    Behavior on margins
    {
        NumberAnimation
        {
            duration: Kirigami.Units.longDuration
            easing.type: Easing.InOutQuad
        }
    }

    Behavior on preferredHeight
    {
        NumberAnimation
        {
            duration: Kirigami.Units.longDuration
            easing.type: Easing.InOutQuad
        }
    }

    Maui.ToolBar
    {
        id: _content
        anchors.fill: parent
        preferredHeight: Maui.Style.toolBarHeightAlt
        spacing: Maui.Style.space.tiny
        position: ToolBar.Footer
        background: Rectangle
        {
            id: _rec
            radius: Maui.Style.radiusV
            opacity: 0.8
            color: Kirigami.Theme.backgroundColor

            Behavior on radius
            {
                NumberAnimation
                {
                    duration: Kirigami.Units.longDuration
                    easing.type: Easing.InOutQuad
                }
            }

            Rectangle
            {
                anchors.fill: parent
                radius: parent.radius
                color: "transparent"
                border.color: Qt.darker(Kirigami.Theme.backgroundColor, 2.7)
                opacity: 0.5

                Rectangle
                {
                    anchors.fill: parent
                    anchors.margins: 1
                    color: "transparent"
                    radius: parent.radius - 0.5
                    border.color: Qt.lighter(Kirigami.Theme.backgroundColor, 2)
                    opacity: 0.7
                }
            }
        }
    }

    Maui.Rectangle
    {
        anchors.fill: parent
        anchors.margins: Maui.Style.space.tiny
        solidBorder: false
        borderColor: Kirigami.Theme.textColor
        borderWidth: 1
        visible: _dropArea.containsDrag
        color: "transparent"
        opacity: 0.3
    }

    DropArea
    {
        id: _dropArea
        anchors.fill: parent
        onDropped: control.contentDropped(drop)
    }

    function isPanelItem(obj)
    {
        return obj instanceof PanelCard
    }

    Component.onCompleted:
    {
        for(var i in _content.leftLayout.children)
        {
            const obj = _content.leftLayout.children[i]
            if(obj.card)
            {
                obj.card.index = popup.count
                obj.card.visible = Qt.binding(function(){return (control.currentCard >= 0 ? control.currentCard === obj.card.index : true) })
                popup.container.insertItem(popup.count, obj.card)
            }
        }

        for(var i in _content.middleLayout.children)
        {
            const obj = _content.middleLayout.children[i]
            if(obj.card)
            {
                obj.card.index = popup.count
                obj.card.visible = Qt.binding(function(){return (control.currentCard >= 0 ? control.currentCard === obj.card.index : true) })
                popup.container.insertItem(popup.count, obj.card)
            }
        }

        for(var i in _content.rightLayout.children)
        {
            const obj = _content.rightLayout.children[i]
            if(obj.card)
            {
                obj.card.index = popup.count
                obj.card.visible = Qt.binding(function(){return (control.currentCard >= 0 ? control.currentCard === obj.card.index : true) })
                popup.container.insertItem(popup.count, obj.card)
            }
        }
    }

    PanelPopup
    {
        id: popup
        visible: opacity > 0.1
        opacity: handler.active ? height/ 200 : 1
        y: control.position === ToolBar.Footer ? 0 - (height + Maui.Style.space.medium) : control.height + Maui.Style.space.medium
        //x: Math.min(0, control.width - width)
        Binding on suggestedHeight
        {
            value:  handler.active ? (control.position === ToolBar.Footer ? 0 - handler.centroid.position.y : handler.centroid.position.y) : popup.height
            restoreMode: Binding.RestoreBinding
        }

        height: suggestedHeight > 200 ? Math.min (_cask.avaliableHeight, popup.implicitHeight) : Math.min(suggestedHeight, handler.active ?  500 : 0)
        width: control.popWidth - x
    }

    DragHandler
    {
        id: handler
        target: null
        grabPermissions: PointerHandler.CanTakeOverFromAnything
    }

    Connections
    {
        target: _cask

        onDesktopPressed:
        {
            control.close()
            mouse.accepted = false
        }
    }


    function close()
    {
        console.log("Close it")
        popup.suggestedHeight = 0
        control.currentCard = -1

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

        popup.suggestedHeight= 500
    }

}
