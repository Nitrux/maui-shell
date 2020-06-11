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
    default property alias content : _content.data
    property alias cards : popup.contentChildren
    property int popWidth : Math.min(_cask.avaliableWidth, Math.max(100, control.width))
    property alias popHeight : popup.height

    property alias backgroundColor: _rec.color
    property alias spacing: _content.spacing

    property bool collapsed : false
    property int position : ToolBar.Footer

    Layout.minimumWidth: implicitWidth
    Layout.margins: Maui.Style.space.medium
    Layout.fillWidth: false

    implicitWidth: collapsed ? 0 : _content.implicitWidth
    implicitHeight: collapsed ? 0 : Maui.Style.toolBarHeightAlt

    Rectangle
    {
        id: _rec
        anchors.fill: parent

        radius: Maui.Style.radiusV
        opacity: 0.8
        color: Kirigami.Theme.backgroundColor
    }

    RowLayout
    {
        id: _content
        anchors.fill: parent
        spacing: Maui.Style.space.tiny
        clip: true
        Item {Layout.fillHeight: true}
    }

    function isPanelItem(obj)
    {
        return obj instanceof PanelCard
    }

    Component.onCompleted:
    {
        for(var i in _content.children)
        {
            const obj = _content.children[i]
            if(obj.card)
            {
                popup.container.insertItem(popup.count, obj.card)
            }
        }
    }

    PanelPopup
    {
        id: popup

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

    }

    function open(index)
    {
        console.log("Pop it up")

        popup.suggestedHeight= 500
    }

}
