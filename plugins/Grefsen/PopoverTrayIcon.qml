import QtQuick 2.5

PopoverPanelItem {
    property string icon: ""
    Image {
        source: "image://icon/" + icon
        sourceSize.width: 64
        sourceSize.height: 64
        anchors.centerIn: parent
    }
}
