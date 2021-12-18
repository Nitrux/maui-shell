import QtQuick 2.12
import QtGraphicalEffects 1.0

import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5
import org.kde.kirigami 2.7 as Kirigami
import org.mauikit.controls 1.2 as Maui
import QtQuick.Templates 2.15 as T

Item
{
    id: control

    default property alias content: _layout.data
    implicitHeight: visible ? _layout.implicitHeight + _rec.topPadding + _rec.bottomPadding : 0

    property int index : -1
    property alias padding : _rec.padding
    property alias background : _rec.background

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

    T.Pane
    {
        id: _rec
        anchors.fill: parent
        padding: Maui.Style.space.medium
        clip: true
        //        anchors.margins: Maui.Style.space.tiny
        background: Rectangle
        {
            color: Kirigami.Theme.backgroundColor
            radius: width <= _cask.availableWidth ? 0 : 10
        }

        contentItem: Column
        {
            id: _layout
        }
    }

    //    layer.enabled: true
    //    layer.effect: OpacityMask
    //    {
    //        maskSource: Item
    //        {
    //            width: control.width
    //            height: control.height

    //            Rectangle
    //            {
    //                anchors.fill: parent
    //                radius: Maui.Style.radiusV
    //            }
    //        }
    //    }

    DropShadow
    {
        visible: !(width <= _cask.availableWidth)
        transparentBorder: true
        anchors.fill: _rec
        horizontalOffset: 0
        verticalOffset: 0
        radius: 8.0
        samples: 17
        color: Qt.rgba(0,0,0,0.2)
        source: _rec
    }


    Component.onDestruction:
    {
        console.log("DESTROY PANEL CARD")
    }

}
