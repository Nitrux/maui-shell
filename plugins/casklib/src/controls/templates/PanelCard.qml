import QtQuick 2.12
import QtGraphicalEffects 1.0

import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5
import org.kde.kirigami 2.7 as Kirigami
import org.mauikit.controls 1.2 as Maui

Item
{
    id: control

    default property alias content: _layout.data
    implicitHeight: visible ? _layout.implicitHeight + _rec.topPadding + _rec.bottomPadding : 0

    property int index : -1
    property alias padding : _rec.padding
    property alias background : _rec.background

    Behavior on implicitHeight
    {
        NumberAnimation
        {
            duration: Kirigami.Units.shortDuration
            easing.type: Easing.OutInQuad
        }
    }

    Pane
    {
        id: _rec
        anchors.fill: parent
        padding: Maui.Style.space.medium
        clip: true
        //        anchors.margins: Maui.Style.space.tiny
        background: Rectangle
        {
            color: Kirigami.Theme.backgroundColor
            radius: isMobile ? 0 : Maui.Style.radiusV
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
        visible: !isMobile
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
