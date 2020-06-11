import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

PopoverPanelItem {

    Item {
        id: clock
        height: time.implicitHeight - 8
        Text {
            x: 2
            id: time
            font.family: "Manzanita"
            font.pixelSize: 46
            color: "lightgreen"
        }
        Text {
            x: 48
            id: seconds
            font.family: "Manzanita"
            font.pixelSize: 30
            color: "lightgreen"
            anchors.baseline: time.baseline
        }
        function update() {
            var now = new Date();
            time.text = ("0" + now.getUTCHours()).slice(-2) + ":"
                    + ("0" + now.getUTCMinutes()).slice(-2)
            seconds.text = ":" + ("0" + now.getUTCSeconds()).slice(-2);
            date.text = now.getFullYear() + "." +
                    ("0" + (now.getMonth() + 1)).slice(-2) + "." +
                    ("0" + now.getDate()).slice(-2)
        }
    }
    Text {
        id: date
        font.pixelSize: 11
        anchors.top: clock.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.horizontalCenterOffset: 2
        color: "beige"
    }
    Timer {
        interval: 1000; running: true; repeat: true
        onTriggered: clock.update()
    }
    Component.onCompleted: clock.update()

    popover: Popover {
        width: 480
        height: 320

        Calendar {
            anchors.fill: parent
            frameVisible: false
            style: CalendarStyle {
                gridVisible: false
                background: Item { }
                dayDelegate: Rectangle {
                    color: styleData.selected ? "#111" :
                                                (styleData.visibleMonth && styleData.valid ? "#444" : "#666");

                    Text {
                        text: styleData.date.getDate()
                        anchors.centerIn: parent
                        color: styleData.valid ? "beige" : "brown"
                        font.pointSize: 14
                    }

                    Rectangle {
                        width: parent.width
                        height: 1
                        color: "#555"
                        anchors.bottom: parent.bottom
                    }

                    Rectangle {
                        width: 1
                        height: parent.height
                        color: "#555"
                        anchors.right: parent.right
                    }
                }
                navigationBar: Rectangle {
                    color: "beige"
                    implicitHeight: row.implicitHeight
                    radius: 6
                    antialiasing: true
                    Row {
                        id: row
                        anchors.fill: parent
                        ToolButton {
                            id: prevButton
                            text: "\u25c0" // BLACK LEFT-POINTING TRIANGLE
                            onClicked: control.showPreviousMonth()
                        }
                        Text {
                            id: navigationBar
                            text: styleData.title
                            width: parent.width - prevButton.width - nextButton.width
                            horizontalAlignment: Text.AlignHCenter
                            anchors.verticalCenter: parent.verticalCenter
                            font.family: "WindsorDemi"
                            font.pointSize: 18
                            color: "#420"
                        }
                        ToolButton {
                            id: nextButton
                            text: "\u25b6" // BLACK RIGHT-POINTING TRIANGLE
                            onClicked: control.showNextMonth()
                        }
                    }
                }
                dayOfWeekDelegate: Text {
                    id: dayLabel
                    text: control.__locale.dayName(styleData.dayOfWeek, control.dayOfWeekFormat)
                    horizontalAlignment: Text.AlignHCenter
                    color: "beige"
                }
            }
        }
    }
}
