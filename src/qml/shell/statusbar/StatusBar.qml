import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5

import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.2 as Maui

import org.maui.cask 1.0 as Cask

import "items"
import "items/calendar"
import "items/player"
import "items/sliders"
import "items/toggles"

Rectangle
{
    id: control
    Layout.fillWidth: true
    implicitHeight: 32
    RowLayout
    {
        anchors.fill: parent

        Cask.PanelSection
        {
            id: _notificationsSection
            Layout.fillWidth: true
            Layout.fillHeight: true
            position : ToolBar.Header
            popWidth: 500
            alignment: Qt.AlignLeft

            CalendarItem
            {
                onClicked: _notificationsSection.open(card.index)
            }

            Cask.PanelItem
            {
                icon.name: "notifications"

                Maui.Badge
                {
                    Layout.fillHeight: true
                    text: _nof.count
                    radius: 4
                }

                onClicked: _notificationsSection.open(card.index)

                card: Cask.PanelCard
                {
                    width: parent.width
                    padding: 0
                    //                    title: _nof.count + " " + qsTr("Notifications")
                    //                    headBar.visible: true
                    //                    headBar.leftContent: ToolButton
                    //                    {
                    //                        icon.name: "configure"
                    //                    }

                    //                    headBar.rightContent: ToolButton
                    //                    {
                    //                        icon.name: "edit-clear-all"
                    //                    }

                    ListView
                    {
                        id: _nof
                        width: parent.width
                        height: Math.min(500, contentHeight)
                        boundsBehavior: ListView.StopAtBounds
                        model: 10
                        spacing: Maui.Style.space.medium
                        delegate: Maui.ItemDelegate
                        {
                            width: parent.width
                            height: 80

                            Maui.ListItemTemplate
                            {
                                anchors.fill: parent
                                iconSource: "documentinfo"
                                label1.text: "Notification Title"
                                label2.text: "Blach some infor about the notification"
                                iconSizeHint: Maui.Style.iconSizes.medium
                                spacing: Maui.Style.space.medium
                            }

                            onClicked: _nof.model--

                        }
                    }
                }
            }
        }

        Cask.PanelSection
        {
            id: _statusSection
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignRight

            position : ToolBar.Header
            alignment: Qt.AlignRight

            popWidth: 500

            TogglesItem
            {
                onClicked: _statusSection.open(card.index)
//                visible: !isMobile
            }

            SlidersItem
            {
                onClicked: _statusSection.open(card.index)
//                visible: !isMobile
            }
            AudioPlayerItem
            {
//                visible: !isMobile
                onClicked: _statusSection.open(card.index)
            }

            Cask.PanelItem
            {
                display: ToolButton.TextBesideIcon
                icon.name: "battery-080"
                text: "80%"
                onClicked: _statusSection.open(card.index)

                card: Cask.PanelCard
                {
                    width: parent.width

                    RowLayout
                    {
                        width: parent.width
                        height: 64
                        Repeater
                        {
                            model: ["system-reboot", "system-shutdown", "system-lock-screen","webcam", "system-suspend"]
                            delegate:  Item
                            {
                                Layout.fillHeight: true
                                Layout.fillWidth: true

                                Kirigami.Icon
                                {
                                    anchors.centerIn: parent
                                    source: modelData
                                    height: Maui.Style.iconSizes.medium
                                    width: height
                                }
                            }
                        }
                    }
                }
            }

            //            NetworkItem
            //            {
            //                onClicked: _statusSection.open(card.index)
            //            }

        }
    }
}

