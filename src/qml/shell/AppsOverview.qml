import QtQuick 2.14
import QtQml 2.14

import QtQuick.Controls 2.14
import QtQuick.Window 2.3
import QtGraphicalEffects 1.0
import QtWayland.Compositor 1.0
import QtQuick.Layouts 1.3
import org.kde.mauikit 1.2 as Maui
import org.kde.kirigami 2.8 as Kirigami
import org.cask.env 1.0 as Env

Maui.Page
{
    id: control
    visible: (overView || (Math.abs(overviewHandler.centroid.position.y) > 50 && count > 0))
    property alias currentIndex : _overviewList.currentIndex
    property alias count : _overviewList.count

    background: FastBlur
    {
        id: fastBlur
        source: _cask
        radius: overView ?  100 :  Math.min(100, overviewHandler.centroid.position.y * -1)
        transparentBorder: false
        cached: true

        Rectangle
        {
            anchors.fill: parent
            color: Kirigami.Theme.backgroundColor
            opacity: (0.5 * parent.radius) /100
        }
    }

    MouseArea
    {
        anchors.fill: parent
        propagateComposedEvents: true
        preventStealing: false
        onClicked:
        {
            overView = false
            showDesktop = true
        }
    }

    footBar.opacity: _overviewList.opacity
    footBar.visible: true
    footBar.middleContent: [
        Button
        {
            text: qsTr("Clear all")
            onClicked:
            {
                overView = false
                _listSurfaces.clear()
            }
        },

        Button
        {
            text: qsTr("Desktop")
            onClicked:
            {
                overView = false
                showDesktop = true
            }
        },

        Button
        {
            text: qsTr("Settings")
        }]

    ListView
    {
        id: _overviewList
        opacity: overView ? 1 : (1* Math.min(200, overviewHandler.centroid.position.y * -1)) / 200
        highlightFollowsCurrentItem: true
        Binding on currentIndex
        {
            delayed: true
            value:  _swipeView.currentIndex
            restoreMode: Binding.RestoreBinding
        }
        height: parent.height * 0.8
        width: parent.width * 0.8
        anchors.centerIn: parent
        orientation: ListView.Horizontal
        model: _listSurfaces
        snapMode: ListView.SnapToItem
        spacing: Maui.Style.space.big


        onCountChanged:
        {
            if(_overviewList.model.count <= 0)
                overView = false
        }

        delegate: ColumnLayout
        {
            width: _overviewList.width * 0.8
            height: _overviewList.height * 0.8
            opacity: (y * -1) > 50 ? 20 / (y * -1): 1
//            anchors.verticalCenter: parent.verticalCenter

            Maui.ListItemTemplate
            {
                Layout.fillWidth: true
                Layout.preferredHeight: Maui.Style.rowHeight
                iconSource: "vvave"
                iconSizeHint: Maui.Style.iconSizes.medium
                label1.text: modelData.toplevel.title + " - " + parent.y
            }

            ItemDelegate
            {
                id: _itemDelegate
                Layout.fillWidth: true
                Layout.fillHeight: true
                background: null
                onClicked:
                {
                    overView = false
                    showDesktop = false
                    _overviewList.currentIndex = index

                }

                clip: false

                Item
                {
                    anchors.fill: parent
                    id: _thumbnail

                    WaylandQuickItem
                    {
                        surface: modelData.surface
                        touchEventsEnabled: false
                        focusOnClick: false
                        enabled: false
                        sizeFollowsSurface: false
                        anchors.fill: parent
                        smooth: true

                        layer.enabled: true
                        layer.effect: OpacityMask
                        {
                            maskSource: Item
                            {
                                width: _thumbnail.width
                                height: _thumbnail.height

                                Rectangle
                                {
                                    anchors.fill: parent
                                    radius: Maui.Style.radiusV
                                }
                            }
                        }
                    }

                    Rectangle
                    {
                        anchors.fill: parent
                        //         anchors.margins: Maui.Style.space.small
                        radius: Maui.Style.radiusV
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

                DropShadow
                {
                    transparentBorder: true
                    anchors.fill: _thumbnail
                    horizontalOffset: 0
                    verticalOffset: 0
                    radius: 8.0
                    samples: 17
                    color: Qt.rgba(0,0,0,0.5)
                    source: _thumbnail
                }

                Maui.Badge
                {
                    id: _closeButton
                    color: hovered || pressed ? Kirigami.Theme.negativeTextColor : Kirigami.Theme.backgroundColor

                    property int position : Maui.App.leftWindowControls.includes("X") ? Qt.AlignLeft : Qt.AlignRight

                    Maui.X
                    {
                        height: Maui.Style.iconSizes.tiny
                        width: height
                        anchors.centerIn: parent
                        color: Kirigami.Theme.textColor
                    }

                    border.color: Kirigami.Theme.textColor

                    anchors
                    {
                        verticalCenter: parent.top
                        horizontalCenter: _closeButton.position === Qt.AlignLeft ? parent.left : parent.right
                    }

                    z: _itemDelegate.z+999
                    onClicked:  modelData.surface.client.close()
                }

            }


            DragHandler
            {
                target: parent
                xAxis.enabled: false
                onActiveChanged:
                {
                    if(!active && (target.y * -1) > 100)
                        modelData.surface.client.close()
                    else target.y = 0
                }
            }
        }
    }
}

