import QtQuick 2.5
import com.theqtcompany.wlprocesslauncher 1.0

MouseArea {
    width: parent.width
    height: background.height

    Rectangle {
        id: background
        color: "red"
        radius: width
        width: height
        height: closeIcon.font.pixelSize * 1.1
        anchors.centerIn: parent
    }

    Text {
        id: closeIcon
        anchors.centerIn: parent
        anchors.verticalCenterOffset: -1
        font.pixelSize: 36
        font.family: "FontAwesome"
        text: "\uf00d"
        color: "white"
    }

    onClicked: Qt.quit()
}
