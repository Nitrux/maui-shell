import QtQuick 2.12
import QtGraphicalEffects 1.0
import QtQml.Models 2.15
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5

import org.mauikit.controls 1.2 as Maui
import QtQuick.Templates 2.15 as T

T.Pane
{
    id: control
    default property alias content: _layout.data
    clip: false

    padding: Maui.Style.contentMargins
    spacing: Maui.Style.defaultSpacing

    implicitHeight: implicitContentHeight + topPadding + bottomPadding

    readonly property bool isOpen: parent !== null && visible
    readonly property int index : ObjectModel.index

    signal opened()
    signal closed()

    opacity: isOpen ? 1 : 0

    //    onVisibleChanged:
    //    {
    //        if(visible)
    //        {
    //            control.opened()
    //        }else
    //        {
    //            control.closed()
    //        }
    //    }

    ListView.onRemove: control.closed()
    ListView.onAdd: control.opened()

    Behavior on opacity
    {
        NumberAnimation
        {
            duration: Maui.Style.units.longDuration
            easing.type: Easing.InOutQuad
        }
    }

    Behavior on implicitHeight
    {
        NumberAnimation
        {
            duration: Maui.Style.units.longDuration
            easing.type: Easing.InOutQuad
        }
    }

    background: Rectangle
    {
        color: Maui.Theme.backgroundColor
        radius: isMobile ? 0 : Maui.Style.radiusV

        Behavior on radius
        {
            NumberAnimation
            {
                duration: Maui.Style.units.longDuration
                easing.type: Easing.InOutQuad
            }
        }

        Behavior on color
        {
            ColorAnimation
            {
                easing.type: Easing.InQuad
                duration: Maui.Style.units.shortDuration
            }
        }

        layer.enabled: true
        layer.effect: DropShadow
        {
            horizontalOffset: 0
            verticalOffset: 0
            samples: 10
            color: Qt.rgba(0,0,0,0.5)
        }
    }

    contentItem: Column
    {
        id: _layout
        clip: true
        spacing: control.spacing
    }

    Component.onDestruction:
    {
        console.log("DESTROY PANEL CARD")
    }


}

