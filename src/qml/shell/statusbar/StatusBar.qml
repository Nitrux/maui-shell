import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5
import org.kde.kirigami 2.7 as Kirigami
import org.mauikit.controls 1.2 as Maui
import org.maui.cask 1.0 as Cask

import "items"
import "items/calendar"

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

            Cask.PanelItem
            {
                id: _togglesItem
                visible: !isMobile
                iconSize: 16
                onClicked: _statusSection.open(card.index)

                Row
                {
                    spacing: Maui.Style.space.medium
                    Kirigami.Icon
                    {
                        source: "settings-configure"
                        height: _togglesItem.iconSize
                        width: height
                        color: _togglesItem.hovered || _togglesItem.down || _togglesItem.pressed ? Kirigami.Theme.highlightColor : Kirigami.Theme.textColor
                    }
                    Kirigami.Icon
                    {
                        source: "network-bluetooth"
                        height: _togglesItem.iconSize
                        width: height
                        color: _togglesItem.hovered || _togglesItem.down || _togglesItem.pressed ? Kirigami.Theme.highlightColor : Kirigami.Theme.textColor
                    }

                    Kirigami.Icon
                    {
                        source: "network-wireless"
                        height: _togglesItem.iconSize
                        width: height
                        color: _togglesItem.hovered || _togglesItem.down || _togglesItem.pressed ? Kirigami.Theme.highlightColor : Kirigami.Theme.textColor
                    }
                }

                card: Cask.PanelCard
                {
                    width: parent.width

                    StackView
                    {
                        id : _togglesStack
                        width: parent.width
                        height: currentItem.implicitHeight
                        clip: true
                        initialItem: Flow
                        {
                            id:_tooglesGrid

                            spacing: Maui.Style.space.medium


                            //                        model: ["network-bluetooth", "input-keyboard-virtual", "rotation-allowed","webcam", "accessories-calculator",  "settings-configure"]
                            ToggleTemplate
                            {
                                icon.name: "network-bluetooth"
                                template.label2.text: "PinePhone-Pro-2x"
                                template.label1.text: i18n("Bluetooth")
                            }

                            ToggleTemplate
                            {
                                icon.name: "continuous"
                            }

                            ToggleTemplate
                            {
                                height: 64
                                icon.name: "network-wireless"
                                template.label2.text: "Camilo-5G"
                                template.label1.text: i18n("WiFi")
                                template.content: Item
                                {
                                    Layout.fillHeight: true
                                    Layout.preferredWidth: height
                                    Kirigami.Icon
                                    {
                                        anchors.centerIn: parent
                                        source: "arrow-right"
                                        height: 16
                                        width: 16
                                    }
                                }

                                onClicked: _togglesStack.push(page)

                                page: Maui.Page
                                {
                                    headBar.farLeftContent: ToolButton
                                    {
                                        icon.name: "go-previous"
                                        onClicked: _togglesStack.pop()
                                    }

                                    implicitHeight: 400
                                }

                            }

                            ToggleTemplate
                            {
                                icon.name:  "mic-ready"
                            }

                            ToggleTemplate
                            {
                                icon.name:  "network-flightmode-on"
                            }

                            ToggleTemplate
                            {
                                icon.name:  "contrast"
                                text: "Dark Mode"
                            }
                        }




                    }
                }
            }

            Cask.PanelItem
            {
                id: _slidersItem
                visible: !isMobile
                iconSize: 16

                onClicked: _statusSection.open(card.index)

                Row
                {
                    spacing: Maui.Style.space.medium
                    Kirigami.Icon
                    {
                        source: "audio-volume-medium"
                        height: _slidersItem.iconSize
                        width: height
                        color: _slidersItem.hovered || _slidersItem.down || _slidersItem.pressed ? Kirigami.Theme.highlightColor : Kirigami.Theme.textColor
                    }

                    Kirigami.Icon
                    {
                        source: "mic-ready"
                        height: _slidersItem.iconSize
                        width: height
                        color: _slidersItem.hovered || _slidersItem.down || _slidersItem.pressed ? Kirigami.Theme.highlightColor : Kirigami.Theme.textColor
                    }

                    Kirigami.Icon
                    {
                        source: "high-brightness"
                        height: _slidersItem.iconSize
                        width: height
                        color: _slidersItem.hovered || _slidersItem.down || _slidersItem.pressed ? Kirigami.Theme.highlightColor : Kirigami.Theme.textColor
                    }
                }

                card: Cask.PanelCard
                {
                    width: parent.width

                    Column
                    {
                        height: 100
                        width: parent.width

                        Label
                        {
                            text: qsTr("Volume")
                        }

                        Slider
                        {
                            width: parent.width
                        }

                        Label
                        {
                            text: qsTr("Brigtness")
                        }


                        Slider
                        {
                            width: parent.width
                        }
                    }


                }
            }

            Cask.PanelItem
            {
                visible: !isMobile
                iconSize: 16

                icon.name: "headphones"
                onClicked: _statusSection.open(card.index)

                card: Cask.PanelCard
                {
                    width: parent.width
                    padding: 0

                    Maui.Page
                    {
                        width: parent.width
                        height: 100
                        Maui.ListItemTemplate
                        {
                            anchors.fill: parent

                            imageSource: "qrc:/Nina Simone_I Put A Spell On You_1965.jpg"
                            imageSizeHint: 90
                            spacing: Maui.Style.space.medium

                            label1.text: "That's Him Over There"
                            label2.text: "Nina Simone"
                        }

                        footBar.visible: true
                        footBar.middleContent: [
                            ToolButton
                            {
                                id: babeBtnIcon
                                icon.name: "love"
                            },

                            Maui.ToolActions
                            {
                                expanded: true
                                autoExclusive: false
                                checkable: false

                                Action
                                {
                                    icon.name: "media-skip-backward"
                                }

                                Action
                                {
                                    text: qsTr("Play and pause")
                                    icon.name: "media-playback-start"
                                }

                                Action
                                {
                                    text: qsTr("Next")
                                    icon.name: "media-skip-forward"
                                }
                            },

                            ToolButton
                            {
                                icon.name: "media-playlist-shuffle"
                            }
                        ]
                    }
                }
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

