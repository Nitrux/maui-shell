import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import org.mauikit.controls as Maui
import org.maui.cask as Cask

import "../../../templates" as Templates

import org.mauikit.calendar as Cal

Cask.PanelItem
{
    id: control
    text: Qt.formatDateTime(new Date(), labelsVisible ? "hh:mm dd/MM" : "hh:mm")

    Component
    {
        id: _tasksPageComponent

        Templates.StackPage
        {
            id: _taskPage
            property date date
            implicitHeight: 400+ topPadding + bottomPadding + headerContainer.implicitHeight + footerContainer.implicitHeight
            title: i18n("Tasks")

            ColumnLayout
            {
                anchors.fill: parent

                Maui.ListItemTemplate
                {
                    Layout.fillWidth: true

                    label1.text: Qt.formatDate(_taskPage.date, "dddd")
                    label1.font.bold: true
                    label1.font.weight: Font.Black
                    label1.font.pointSize: Maui.Style.fontSizes.big
                    label2.text: Qt.formatDate(_taskPage.date, "dd MMMM yyyy")
                }

                Maui.ListBrowser
                {
                    id: _listView
                    implicitHeight: contentHeight
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    Maui.Holder
                    {
                        anchors.fill: parent
                        visible: _listView.count === 0
                        title: i18n("Nothing here")
                        body: i18n("No output or input devices found.")
                        emoji: "code-context"
                    }
                }
            }
        }
    }

    card: Cask.PanelCard
    {
        id: _card
        width: ListView.view.width

        StackView
        {
            id: _stack

            width: parent.width
            height: currentItem.implicitHeight

            initialItem: Cal.DaysGrid
                {
                    id: _monthGrid

                    width: parent.width
                    implicitHeight: 260

                    header: Maui.ListItemTemplate
                    {
                        width: parent.width
                        label1.text: control.text
                        label1.font.bold: true
                        label1.font.weight: Font.Black
                        label1.font.pointSize: Maui.Style.fontSizes.big
                        label2.text: Qt.formatDate(new Date(), "ddd MMMM yyyy")
                        ToolButton
                        {
                            icon.name: "go-next"
                            onClicked: _stack.push(_tasksPageComponent)
                        }
                    }
                }

        }
    }

    Timer
    {
        interval: 60000; running: true; repeat: true
        onTriggered: control.text = Qt.formatDateTime(new Date(), labelsVisible ? "hh:mm dd/MM" : "hh:mm")
    }
}
