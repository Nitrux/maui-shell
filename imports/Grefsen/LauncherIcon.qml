import QtQuick 2.5
import com.theqtcompany.wlprocesslauncher 1.0

MouseArea {
    width: parent.width
    height: width

    ProcessLauncher {
        id: launcher
    }

    property string path: ""
    property string icon: ""
    Image {
        source: "image://icon/" + icon
        sourceSize.width: 64
        sourceSize.height: 64
        anchors.centerIn: parent
    }
    onClicked: if (path) launcher.launch(path); else console.log("nothing to launch")
}
