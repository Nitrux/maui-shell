import QtQuick 2.5
import QtQuick.Controls 2.0
import Grefsen 1.0

HoverArea {
    id: root
    width: 200
//    height: Math.min(1000, list.implicitHeight)
    height: 500
    signal close
    onExited: root.close()

    Rectangle {
        anchors.fill: parent
        border.color: "black"
        color: "#555"
        radius: 10
        opacity: 0.5
    }

    TextField {
        id: searchField
        width: parent.width - 12
        x: 6; y: 6
        focus: true
        onTextChanged: LauncherModel.substringFilter = text
        MouseArea {
            height: searchField.height
            width: height
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            onClicked: {
                LauncherModel.reset()
                searchField.text = ""
            }
            Text {
                anchors.centerIn: parent
                font.family: "FontAwesome"
                text: "\uf05c"  // TODO more specific clear-text-field icon
                font.pointSize: searchField.font.pointSize * 1.25
            }
        }

    }
    ListView {
        id: list
        anchors.fill: parent
        anchors.topMargin: searchField.height + 6
        anchors.bottomMargin: 0
        anchors.margins: 6
        clip: true
        model: LauncherModel.applicationMenu
        delegate: MouseArea {
            width: parent.width
            height: 32
            onClicked: LauncherModel.select(modelData)

            Rectangle {
                radius: 2
                anchors.fill: parent
                anchors.margins: 1
                opacity: 0.35
            }
            Image {
                source: "image://icon/" + modelData.icon
                sourceSize.width: 22
                sourceSize.height: 22
                anchors.verticalCenter: parent.verticalCenter
                x: 4
            }
            Text {
                anchors.verticalCenter: parent.verticalCenter
                x: 30
                elide: Text.ElideRight
                text: modelData.title
//                Component.onCompleted: console.log(" "+ JSON.stringify(modelData))
                width: parent.width - x - 4
            }
        }
    }
}
