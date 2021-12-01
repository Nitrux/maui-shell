import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5
import org.kde.kirigami 2.7 as Kirigami
import org.mauikit.controls 1.2 as Maui
import org.maui.cask 1.0 as Cask
import Qt.labs.calendar 1.0

Cask.PanelItem
{
    id: control
    icon.name: "clock"
    font.bold: true
    text: new Date().toTimeString()
    card: Cask.PanelCard
    {
        width: ListView.view.width
        padding: 0
//        title: new Date().toDateString()
//        headBar.visible: true

        Item
        {
            width: parent.width
            height: 250
        MonthGrid
                   {
                       id: _monthGrid
                        width: parent.width
                        height: implicitHeight
                        anchors.centerIn: parent

                       month: new Date().getMonth()
                       year: new Date().getFullYear()
                       locale: Qt.locale("en_US")
                       delegate: Label
                       {
                           horizontalAlignment: Text.AlignHCenter
                           verticalAlignment: Text.AlignVCenter
                           opacity: model.month === _monthGrid.month ? 1 : 0
                           text: model.day
                           font: _monthGrid.font
                           color: model.day ===  new Date().getDay() ?  Kirigami.Theme.highlightColor : Kirigami.Theme.textColor
                       }
                   }
        }

    }

    Timer {
        interval: 1000; running: true; repeat: true
        onTriggered: control.text = new Date().toTimeString()
    }
}
