import QtQuick 2.12
import QtGraphicalEffects 1.0

import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5
import org.kde.kirigami 2.7 as Kirigami
import org.mauikit.controls 1.2 as Maui
import QtQuick.Templates 2.15 as T

T.Pane
{
    id: control
    padding: Maui.Style.space.medium
    clip: false
    spacing: Maui.Style.space.medium
    default property alias content: _layout.data
    implicitHeight: visible ? implicitContentHeight + topPadding + bottomPadding : 0

    property int index : -1

    signal opened()
    signal closed()

    opacity: visible ? 1 : 0

    onVisibleChanged:
    {
        if(visible)
        {
            control.opened()
        }else
        {
            control.closed()
        }
    }

    Behavior on opacity
    {
        NumberAnimation
        {
            duration: Kirigami.Units.longDuration
            easing.type: Easing.InOutQuad
        }
    }

    Behavior on implicitHeight
    {
        NumberAnimation
        {
            duration: Kirigami.Units.longDuration
            easing.type: Easing.InOutQuad
        }
    }

    background: Rectangle
    {
        color: Kirigami.Theme.backgroundColor
        radius: isMobile ? 0 : 10

        Behavior on radius
        {
            NumberAnimation
            {
                duration: Kirigami.Units.longDuration
                easing.type: Easing.InOutQuad
            }
        }

        layer.enabled: true
        layer.effect: DropShadow
        {
            transparentBorder: true
            horizontalOffset: 0
            verticalOffset: 0
            radius: 8.0
            samples: 17
            color: Qt.rgba(0,0,0,0.2)
        }
    }

    contentItem: Column
    {
        id: _layout
        spacing: control.spacing
    }

    Component.onDestruction:
    {
        console.log("DESTROY PANEL CARD")
    }
}

