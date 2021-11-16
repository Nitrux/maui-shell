import QtQuick 2.1
import QtGraphicalEffects 1.0

Flickable {
    id: root
    width: 100
    height: 480
    anchors.right: parent.right
    y: -10
    z: 10000
    contentWidth: width * 2
    contentHeight: height

    function open() {
        contentX = width
    }

    default property alias __content: rect.data
    onDraggingChanged: if (!dragging) {
       if (horizontalVelocity > 0)
           flick(-1500, 0)
       else
           flick(1500, 0)
    }
    flickableDirection: Flickable.HorizontalFlick
    rebound: Transition {
        NumberAnimation {
            properties: "x"
            duration: 300
            easing.type: Easing.OutBounce
        }
    }

    RectangularGlow {
        id: effect
        x: width
        width: root.width
        height: root.height
        glowRadius: 10
        spread: 0.2
        color: "cyan"
        cornerRadius: rect.radius + glowRadius

        Rectangle {
            id: rect
            anchors.fill: parent
            anchors.rightMargin: -200
            color: "black"
            radius: 10
            antialiasing: true
        }
    }
}
