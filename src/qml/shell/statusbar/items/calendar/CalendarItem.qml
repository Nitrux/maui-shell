import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5
import org.kde.kirigami 2.7 as Kirigami
import org.kde.mauikit 1.2 as Maui
import org.maui.cask 1.0 as Cask
import Qt.labs.calendar 1.0

Cask.PanelItem
{
    id: control
    text: new Date().toTimeString()
    font.bold: true
    font.weight: Font.Bold
    card: Cask.PanelCard
    {
        padding: 0
        title: listview.currentItem.month + " - " + listview.currentItem.year
        headBar.visible: true
        alignment: control.parent.alignment

        ListView
        {
            id: listview
            width: parent.width
            height: 250
            snapMode: ListView.SnapOneItem
            orientation: ListView.Horizontal
            highlightFollowsCurrentItem: true
            highlightRangeMode: ListView.StrictlyEnforceRange

            model: CalendarModel
            {
                from: new Date(2018, 0, 1)
                to: new Date(2022, 11, 31)
            }

            delegate: MonthGrid
            {
                id: _monthGrid
                width: listview.width
                height: listview.height

                month: model.month
                year: model.year
                locale: Qt.locale("en_US")
                delegate: Label
                {
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    opacity: model.month === _monthGrid.month ? 1 : 0
                    text: model.day
                    font: _monthGrid.font
                    color: Kirigami.Theme.textColor
                }
            }

            ScrollIndicator.horizontal: ScrollIndicator { }
        }
    }

    Timer {
        interval: 1000; running: true; repeat: true
        onTriggered: control.text = new Date().toTimeString()
    }
}
