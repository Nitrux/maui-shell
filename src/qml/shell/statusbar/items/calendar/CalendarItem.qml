import QtQuick 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5
import QtQuick.Templates 2.15 as T

import org.mauikit.controls 1.3 as Maui
import org.maui.cask 1.0 as Cask

import "../../../templates" as Templates

import org.maui.calendar 1.0 as Cal

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
            implicitHeight: 400 + topPadding + bottomPadding + headerContainer.implicitHeight + footerContainer.implicitHeight
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

        T.StackView
        {
            id: _stack

            width: parent.width
            height: currentItem.implicitHeight

            initialItem: Column
            {
                spacing: _card.spacing
                Maui.ListItemTemplate
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

                Cal.MonthView
                {
                    id: _monthGrid

                    width: parent.width
                    height: 200

//                    month: new Date().getMonth()
//                    year: new Date().getFullYear()
//                    locale: Qt.locale("en_US")

//                    delegate: T.ItemDelegate
//                    {
//                        id: _delegate
//                        implicitWidth: implicitContentWidth + rightPadding + leftPadding

//                        implicitHeight: implicitContentHeight + topPadding + bottomPadding

//                        background: Rectangle
//                        {
//                            radius: Maui.Style.radiusV
//                            color: model.today || _delegate.hovered || _delegate.down ? Maui.Theme.highlightColor : "transparent"
//                        }

//                        contentItem: Label
//                        {
//                            horizontalAlignment: Text.AlignHCenter
//                            verticalAlignment: Text.AlignVCenter
//                            opacity: model.month === _monthGrid.month ? 1 : 0.4
//                            text: String(model.day)
//                            font: _monthGrid.font
//                            color: model.today || _delegate.hovered ? Maui.Theme.highlightedTextColor : Maui.Theme.textColor
//                        }

//                        onClicked: _stack.push(_tasksPageComponent, {'date': model.date})
//                    }
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
