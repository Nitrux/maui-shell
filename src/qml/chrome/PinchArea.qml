
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import QtQml 2.15

import QtWayland.Compositor 1.15
import QtGraphicalEffects 1.15

import org.mauikit.controls 1.3 as Maui

Item
{
    property alias active : pinch4.active
    property alias activeScale : pinch4.activeScale

    PinchHandler
    {
        id: pinch4
        objectName: "4-finger pinch"
        minimumPointCount: 4
        maximumPointCount: 4
        minimumScale: 0.5
        maximumScale: 2
        minimumRotation: 0
        maximumRotation: 0

        onActiveChanged:
        {
            if (!active)
            {
                // just a silly way of getting a QSize for sendConfigure()

                if(activeScale <= minimumScale)
                {
                    surfaceItem.surface.client.close()
                }

                if(width * scale > availableGeometry.width*0.9 && height * scale > availableGeometry.height*0.9)
                {
                    window.maximize()
                    rootChrome.scale = 1
                    rootChrome.x =0
                    rootChrome.y = 0

                    return;
                }

                var minWidth = availableGeometry.width/2
                var minHeight = availableGeometry.height/2
                if(width*scale <= minWidth && height *scale < minHeight)
                {

                    rootChrome.scale = 1


                    return;
                }

                var size = toplevel.sizeForResize(Qt.size(width * scale, height * scale), Qt.point(0, 0), 0);
                toplevel.sendConfigure(size, [3] /*XdgShellToplevel.ResizingState*/);
                rootChrome.scale = 1
                rootChrome.x = pinch4.centroid.scenePosition.x -(size.width/2)
                rootChrome.y = pinch4.centroid.scenePosition.y-(size.height/2)
            }
        }
    }
}
