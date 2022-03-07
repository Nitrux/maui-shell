import QtQuick 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5
import QtQuick.Templates 2.15 as T

import org.kde.kirigami 2.7 as Kirigami
import org.mauikit.controls 1.3 as Maui
import org.maui.cask 1.0 as Cask
import Qt.labs.calendar 1.0
import "../../../templates" as Templates

Cask.PanelItem
{
    id: control
    font.bold: true
    text: Qt.formatDateTime(new Date(), "hh:mm")


    Component
    {
        id: _tasksPageComponent

        Templates.StackPage
        {

            implicitHeight: Math.max(400, Math.min(400, _listView.implicitHeight + topPadding + bottomPadding + headerContainer.implicitHeight + footerContainer.implicitHeight + Maui.Style.space.big))
            title: i18n("Tasks")

            Maui.Holder
            {
                anchors.fill: parent
                visible: _listView.count === 0
                title: i18n("Nothing here")
                body: i18n("No output or input devices found.")
                emoji: "code-context"
            }

            Maui.ListBrowser
            {
                id: _listView
                implicitHeight: contentHeight

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

                MonthGrid
                {
                    id: _monthGrid

                    width: parent.width
                    height: 200

                    month: new Date().getMonth()
                    year: new Date().getFullYear()
                    locale: Qt.locale("en_US")
                    delegate: T.ItemDelegate
                    {
                        id: _delegate
                        enabled: model.day
                        implicitWidth: implicitContentWidth + rightPadding + leftPadding

                        implicitHeight: implicitContentHeight + topPadding +bottomPadding

                        background: Rectangle
                        {
                            radius: 10
                            color: _delegate.hovered || _delegate.down ? Kirigami.Theme.highlightColor : "transparent"
                        }

                        contentItem: Label
                    {
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        opacity: model.month === _monthGrid.month ? 1 : 0
                        text: model.day
                        font: _monthGrid.font
                        color: model.today ?  Kirigami.Theme.highlightColor : (_delegate.hovered ? Kirigami.Theme.highlightedTextColor : Kirigami.Theme.textColor)
                    }

                        onClicked: _stack.push(_tasksPageComponent)
                    }
                }
            }
        }
    }


    Timer
    {
        interval: 60000; running: true; repeat: true
        onTriggered: control.text = Qt.formatDateTime(new Date(), "hh:mm")

    }
}
