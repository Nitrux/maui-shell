import QtQuick 2.12
import QtGraphicalEffects 1.0
 import QtQml.Models 2.15
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5
import org.kde.kirigami 2.7 as Kirigami
import org.mauikit.controls 1.2 as Maui
import QtQuick.Templates 2.15 as T

T.Pane
{
    id: control
    default property alias content: _layout.data

    padding: Maui.Style.space.medium
    clip: false
    spacing: Maui.Style.space.medium
    implicitHeight: implicitContentHeight + topPadding + bottomPadding
//    visible:
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
Label
{
    color: "orange"
    text: control.visible
}
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
        clip: true
        spacing: control.spacing
    }

    Component.onDestruction:
    {
        console.log("DESTROY PANEL CARD")
    }


}

