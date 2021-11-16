import QtQuick 2.5

Rectangle {
    color: "black"
    opacity: 0.8
    radius: 15
    antialiasing: true
    visible: false

    Shortcut {
        sequence: "Meta+T"
        onActivated: visible = !visible
    }

    MultiPointTouchArea {
        id: mpta
        anchors.fill: parent
        touchPoints: [
            TouchPoint { property color color: "red" },
            TouchPoint { property color color: "orange" },
            TouchPoint { property color color: "lightsteelblue" },
            TouchPoint { property color color: "green" },
            TouchPoint { property color color: "blue" },
            TouchPoint { property color color: "violet" },
            TouchPoint { property color color: "steelblue" },
            TouchPoint { property color color: "magenta" },
            TouchPoint { property color color: "goldenrod" },
            TouchPoint { property color color: "darkgray" }
        ] }

    Repeater {
        model: 10

        Item {
            id: crosshairs
            property TouchPoint touchPoint
            x: touchPoint.x - width / 2
            y: touchPoint.y - height / 2
            width: 300; height: 300
            visible: touchPoint.pressed
            rotation: touchPoint.rotation

            Rectangle {
                color: touchPoint.color
                anchors.centerIn: parent
                width: 2; height: parent.height
                antialiasing: true
            }
            Rectangle {
                color: touchPoint.color
                anchors.centerIn: parent
                width: parent.width; height: 2
                antialiasing: true
            }
            Rectangle {
                color: touchPoint.color
                implicitWidth: label.implicitWidth + 8
                implicitHeight: label.implicitHeight + 16
                radius: width / 2
                anchors.centerIn: parent
                antialiasing: true
                Rectangle {
                    color: "black"
                    opacity: 0.35
                    width: (parent.width - 8) * touchPoint.pressure
                    height: width
                    radius: width / 2
                    anchors.centerIn: parent
                    antialiasing: true
                }
                Rectangle {
                    color: "transparent"
                    border.color: "white"
                    border.width: 2
                    opacity: 0.75
                    visible: width > 0
                    width: touchPoint.ellipseDiameters.width
                    height: touchPoint.ellipseDiameters.height
                    radius: Math.min(width, height) / 2
                    anchors.centerIn: parent
                    antialiasing: true
                }
                Text {
                    id: label
                    anchors.centerIn: parent
                    color: "white"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    property string uid: touchPoint.uniqueId === undefined || touchPoint.uniqueId.numericId === -1 ?
                                             "" : "\nUID " + touchPoint.uniqueId.numericId
                    text: "x " + touchPoint.x.toFixed(1) +
                          "\ny " + touchPoint.y.toFixed(1) + uid +
                          "\nID " + touchPoint.pointId.toString(16) +
                          "\n∡" + touchPoint.rotation.toFixed(1) + "°"
                }
            }
            Rectangle {
                id: velocityVector
                visible: width > 0
                width: touchPoint.velocity.length()
                height: 4
                Behavior on width { SmoothedAnimation { duration: 200 } }
                radius: height / 2
                antialiasing: true
                color: "gray"
                x: crosshairs.width / 2
                y: crosshairs.height / 2
                rotation: width > 0 ? Math.atan2(touchPoint.velocity.y, touchPoint.velocity.x) * 180 / Math.PI - crosshairs.rotation : 0
                Behavior on rotation { SmoothedAnimation { duration: 20 } }
                transformOrigin: Item.BottomLeft
            }

            Component.onCompleted: touchPoint = mpta.touchPoints[index]
        }
    }
}
