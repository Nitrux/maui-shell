import QtQuick
import QtQuick.Controls

import org.mauikit.controls as Maui
import org.maui.cask as Cask

Item
{
    id: control
    default property alias content: _content.data
    readonly property alias container: _content
    readonly property alias overlayContainer : _overlayContent
    readonly property alias superOverlay: _superOverlay
    property alias overlayContent : _overlayContent.data

    readonly property int avaliableHeight : _content.height - _topPanelContainer.height
    readonly property int avaliableWidth : _content.width

    readonly property rect availableGeometry : Qt.rect(0, _topPanelContainer.height, avaliableWidth, avaliableHeight)

    readonly property alias contentY: _content.y
    readonly property alias surface : _content

    property alias background : _rec

    property alias overlayTopPanel : _overlayTopPanel
    property alias overlay : _overlay

    property alias underneathContent : _underneathContent.data
    property alias backgroundColor : _rec.color
    property alias backgroundImage: _img.source

    property alias topPanel: _topPanel

    Rectangle
    {
        id: _rec
        anchors.fill: parent
        color: "#333"

        Item
        {
            id: _bg
            anchors.fill: parent
            Image
            {
                id: _img
                anchors.fill: parent
                fillMode: Image.PreserveAspectCrop
            }
        }
    }

    Item
    {
        id: _superOverlay
        anchors.fill: parent

        Item
        {
            id: _underneathContent
            anchors.fill: parent
        }

        Item
        {
            id: _content
            anchors.fill: parent
        }

        Item
        {
            id: _overlay
            anchors.fill: parent
        }

        Item
        {
            id: _overlayContent
            anchors.fill: parent
        }
    }

    Item
    {
        id: _overlayTopPanel
        //        z: _content.z+1
        anchors.fill: parent
    }

    Item
    {
        id: _topPanelContainer
        //            color: "yellow"
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: _topPanel.visible ? _topPanel.implicitHeight : 0
        //        color : "purple"

        Cask.Panel
        {
            id: _topPanel
            anchors.fill: parent
        }
    }

    Component.onDestruction:
    {
        console.log("DESTROY DASHBOARD")
    }
}
