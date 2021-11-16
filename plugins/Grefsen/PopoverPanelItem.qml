import QtQuick 2.5

MouseArea {
    id: root
    width: parent.width
    height: width

    property Component popover: null

    Loader {
        id: loader
        anchors.left: root.right
        anchors.top: root.top
        onItemChanged: if (item) item.pointLeftToY = root.height / 2
    }

    onClicked: {
        if (loader.sourceComponent == undefined)
            loader.sourceComponent = popover
        else
            loader.sourceComponent = undefined
    }
}
