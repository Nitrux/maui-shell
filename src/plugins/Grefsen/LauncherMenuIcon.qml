import QtQuick 2.5
import Grefsen 1.0

MouseArea {
    id: root
    width: parent.width
    height: width
    property bool checked: false

    Image {
        source: "qrc:///images/grefsen-logo-on-silhouette.png"
        anchors.centerIn: parent
    }
    onClicked: root.checked = !checked

    LauncherMenu {
        anchors.left: parent.right
        visible: root.checked
        onClose: root.checked = false
    }
}
