import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QtWayland.Compositor

import org.mauikit.controls as Maui

Control
{
    id:  control

    implicitHeight: _content.implicitHeight + topPadding + bottomPadding

    padding: Maui.Style.space.small
    background: null
    //    font.bold: true

    hoverEnabled: !Maui.Handy.isMobile

    contentItem: Item
    {
        id: _content
        implicitHeight: _layout.implicitHeight       

        DragHandler
        {
            id: titlebarDrag
            grabPermissions: TapHandler.CanTakeOverFromAnything
            target: rootChrome.moveItem
            yAxis.maximum: rootChrome.parent.height
            yAxis.minimum: 0
            cursorShape: Qt.ClosedHandCursor
            onActiveChanged:
            {
                if(toplevel.maximized)
                {
                   rootChrome. window.unmaximize()
                }

                if(!rootChrome.activated)
                {
                    rootChrome.window.activate()
                }
            }
        }

        TapHandler
        {
            onTapped: rootChrome.window.activate()
        }

        HoverHandler
        {
            enabled: rootChrome.activated
            cursorShape: Qt.OpenHandCursor
        }

        RowLayout
        {
            id: _layout
            anchors.fill: parent
            spacing: control.spacing

            Label
            {
                // opacity: control.hovered ? 1 : 0
                Layout.fillWidth: true
                text: rootChrome.title
                horizontalAlignment: Qt.AlignLeft
                elide: Text.ElideMiddle
                wrapMode: Text.NoWrap
                font: control.font
            }

            Maui.CSDControls
            {
                onButtonClicked: performActiveWindowAction(type)
                isActiveWindow: rootChrome.activated
                maximized: rootChrome.toplevel.maximized
            }
        }
    }

    function performActiveWindowAction(type)
    {
        if (type === Maui.CSDButton.Close)
        {
            window.close()
        }
        else if (type === Maui.CSDButton.Maximize)
        {
            window.maximize()
        }
        else if (type === Maui.CSDButton.Restore)
        {
            window.unmaximize()
        }
        else if (type ===  Maui.CSDButton.Minimize)
        {
            window.minimize()
        }
    }
}
