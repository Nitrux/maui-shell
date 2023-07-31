import QtQuick
import QtQuick.Controls

import QtWayland.Compositor
import Qt5Compat.GraphicalEffects

import org.mauikit.controls as Maui
import org.maui.cask as Cask

Rectangle
{
    id: control

    visible: rootChrome.decorationVisible || _dropShadowHelper.radius > -1

    Maui.Theme.inherit: false
    Maui.Theme.colorSet: Maui.Theme.Header

    radius: window.maximized ? 0 : (rootChrome.decorationVisible ? Maui.Style.radiusV :  _dropShadowHelper.radius)

    color: Maui.Theme.backgroundColor

    readonly property int titleBarHeight : _titleBarLoader.item ? _titleBarLoader.item.height : 0

    property real resizeAreaWidth: 12
    property bool canResize : true
    property bool showDropShadow: visible

    property alias showTitleBar : _titleBarLoader.active
    property alias showResizeHandlers : _resizeHandlersLoader.active

    signal requestResize(var size)

    Cask.DropShadowHelper
    {
        id: _dropShadowHelper
        appId: rootChrome.appId
    }

    MouseArea
    {
        anchors.fill: parent
        propagateComposedEvents: false
        preventStealing: true
        onPressed:
        {
            mouse.accepted = true
        }
    }

    //        FastBlur
    //        {
    //            anchors.fill: parent
    //            radius: 64
    //            opacity: 0.4
    //            source: ShaderEffectSource
    //            {
    //                id: _shader
    //                property rect area : sourceItem.mapToItem(rootChrome, rootChrome.x, rootChrome.y, rootChrome.width, rootChrome.height)
    //                format: ShaderEffectSource.RGB
    //                sourceItem: surfaceArea
    //                recursive: false
    //                sourceRect: Qt.rect(rootChrome.x, rootChrome.y, rootChrome.width, rootChrome.height)

    //            }

    //            layer.enabled: true
    //            layer.effect: Desaturate
    //            {
    //                desaturation: -1.2
    //            }
    //        }

    Loader
    {
        id: _titleBarLoader

        asynchronous: true

        visible: !surfaceItem.isPopup && active

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        sourceComponent: TitleBar {}
    }

    Loader
    {
        id: _resizeHandlersLoader

        asynchronous: true

        anchors.fill: parent

        sourceComponent: Item
        {
            // TODO write a ResizeHandler for this purpose? otherwise there are up to 8 components for edges and corners
            Item
            {
                id: rightEdgeResizeArea

                enabled: control.canResize

                focus: false
                x: parent.width - resizeAreaWidth / 2;
                width: resizeAreaWidth; height: parent.height - resizeAreaWidth

                onXChanged:
                {
                    if (horzDragHandler.active)
                    {
                        var size = toplevel.sizeForResize(horzDragHandler.initialSize,
                                                          Qt.point(horzDragHandler.translation.x,
                                                                   horzDragHandler.translation.y),
                                                          Qt.RightEdge);
                        control.requestResize(size)
                    }
                }

                DragHandler
                {
                    id: horzDragHandler
                    property size initialSize
                    onActiveChanged: if (active) initialSize = Qt.size(rootChrome.width, rootChrome.height)
                    yAxis.enabled: false
                }

                HoverHandler
                {
                    id: rightEdgeHover
                    cursorShape: Qt.SizeHorCursor // problem: this so far only sets the EGLFS cursor, not WaylandCursorItem
                }
            }

            Item
            {
                id: bottomEdgeResizeArea

                enabled: control.canResize

                focus: false

                y: parent.height - resizeAreaWidth / 2; height: resizeAreaWidth; width: parent.width - resizeAreaWidth

                onYChanged:
                {
                    if (vertDragHandler.active)
                    {
                        var size = toplevel.sizeForResize(vertDragHandler.initialSize,
                                                          Qt.point(vertDragHandler.translation.x, vertDragHandler.translation.y),
                                                          Qt.BottomEdge);
                       control.requestResize(size)
                    }
                }

                DragHandler
                {
                    id: vertDragHandler
                    property size initialSize
                    onActiveChanged: if (active) initialSize = Qt.size(control.width, control.height)
                    xAxis.enabled: false
                }

                HoverHandler
                {
                    id: bottomEdgeHover
                    cursorShape: Qt.SizeVerCursor
                }
            }

            Item
            {
                id: bottomRightResizeArea

                enabled: control.canResize

                focus: false

                x: parent.width - resizeAreaWidth / 2; y: parent.height - resizeAreaWidth / 2

                width: resizeAreaWidth;
                height: parent.height - resizeAreaWidth

                onXChanged: resize()
                onYChanged: resize()

                function resize()
                {
                    if (bottomRightDragHandler.active) {
                        var size = toplevel.sizeForResize(bottomRightDragHandler.initialSize,
                                                          Qt.point(bottomRightDragHandler.translation.x, bottomRightDragHandler.translation.y),
                                                          Qt.BottomEdge | Qt.RightEdge);
                        control.requestResize(size)
                    }
                }

                DragHandler
                {
                    id: bottomRightDragHandler
                    property size initialSize
                    onActiveChanged: if (active) initialSize = Qt.size(control.width, control.height)
                }

                HoverHandler
                {
                    id: bottomRightHover
                    cursorShape: Qt.SizeFDiagCursor
                }
            }
            // end of resizing components
        }
    }

    layer.enabled: control.showDropShadow
    layer.effect: DropShadow
    {
        transparentBorder: true
        horizontalOffset: 0
        verticalOffset: 0
        radius: 12
        samples: 17
        color: "#000000"
    }
}

