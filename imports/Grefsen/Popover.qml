import QtQuick 2.6

Item {
    id: wrapper
    property real pointAboveToX: -1
    property real pointBelowToX: -1
    property real pointLeftToY: -1
    property real pointRightToY: -1
    width: 100
    height: 100

    default property alias __content: contentContainer.data
    property var __popoverPos: mapToItem(glassPane, x, y)
    Component.onCompleted: popover.parent = glassPane

    Item {
        id: popover
        x: __popoverPos.x
        y: __popoverPos.y
        width: wrapper.width
        height: wrapper.height

        Rectangle {
            id: outerBackground
            anchors.fill: parent
            color: "#555"
            radius: 10
            antialiasing: true
            opacity: 0.5
        }

        Rectangle {
            id: pointyBitBorder
            color: "black"
            border.color: innerBackground.border.color
            border.width: 2
            anchors.fill: pointyBit
            anchors.margins: -2
            rotation: 45
            antialiasing: true
        }

        Rectangle {
            id: innerBackground
            anchors.fill: parent
            anchors.margins: 4
            color: "#333"
            border.width: 2
            border.color: "#FF6600"
            radius: 10
            antialiasing: true
        }

        Rectangle {
            id: pointyBit
            property real pointToX: __popoverPos.x + Math.max(pointAboveToX, pointBelowToX)
            x: pointToX > 0 ? pointToX - popover.x - 2 : pointRightToY > 0 ? popover.width - 12 : -2
            property real pointToY: __popoverPos.y + Math.max(pointLeftToY, pointRightToY)
            y: pointToY > 0 ? pointToY - popover.y - 2 : pointBelowToX > 0 ? popover.height - 12 : -5
            width: 14
            height: 14
            rotation: 45
            color: innerBackground.color
            antialiasing: true
            visible: pointToX > 0 || pointToY > 0
        }

        Rectangle {
            id: contentContainer
            radius: 8
            anchors.fill: innerBackground
            anchors.margins: 6
            color: "transparent"
            clip: true // maybe expensive, but done because SwipeView lets neighbor panels stay visible sometimes
        }
    }
}
