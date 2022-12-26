import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import QtQml 2.15

import QtWayland.Compositor 1.15
import QtGraphicalEffects 1.15

import org.mauikit.controls 1.3 as Maui

Control
{
    id:  control

    implicitHeight: _content.implicitHeight + topPadding + bottomPadding

    padding: Maui.Style.space.medium
    background: null

    //    font.bold: true

    hoverEnabled: !Maui.Handy.isMobile

    contentItem: MouseArea
    {
        id: _content
        implicitHeight: _layout.implicitHeight

        onPressed:
        {
            rootChrome.window.activate()
            mouse.accepted= false
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
                opacity: control.hovered ? 1 : 0
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

        DragHandler
        {
            id: titlebarDrag
            grabPermissions: PointerHandler.TakeOverForbidden
            target: rootChrome
            yAxis.maximum: rootChrome.parent.height
            yAxis.minimum: 0
            cursorShape: Qt.ClosedHandCursor
            onActiveChanged:
            {
                if(toplevel.maximized)
                {
                    window.unmaximize()
                }

                if(!rootChrome.activated)
                {
                    rootChrome.window.activate()
                }
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
