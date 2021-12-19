import QtQuick 2.15
import QtWayland.Compositor 1.0

WaylandQuickItem
{
    id: cursorItem
    property QtObject seat
    property int hotspotX: 0
    property int hotspotY: 0

    visible: cursorItem.surface != null
    inputEventsEnabled: false
    enabled: false
    transform: Translate {
        // If we've set an output scale factor different from the device pixel ratio
        // then the item will be rendered scaled, so we need to shift the hotspot accordingly
        x: -hotspotX * (output ? output.scaleFactor / Screen.devicePixelRatio : 1)
        y: -hotspotY * (output ? output.scaleFactor / Screen.devicePixelRatio : 1)
    }

    Connections
    {
        target: seat
        function onCursorSurfaceRequest(surface, hotspotX, hotspotY)
        {
            console.log("CURSOR SURFACE REQUESTED")
            cursorItem.surface = surface;
            cursorItem.hotspotX = hotspotX;
            cursorItem.hotspotY = hotspotY;
        }
    }

    WaylandQuickItem {
        id: dragIcon
        property point offset
        inputEventsEnabled: false

        x: cursorItem.hotspotX + offset.x
        y: cursorItem.hotspotY + offset.y
        z: 999999
        surface: cursorItem.seat ? cursorItem.seat.drag.icon : null

        Connections {
            target: dragIcon.surface
            function onOffsetForNextFrame(offset)
            {
                console.log("OFFSET NEXT FRAME")

                dragIcon.offset = offset;
            }
        }
    }
}
