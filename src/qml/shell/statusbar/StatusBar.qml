import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5
import org.kde.kirigami 2.7 as Kirigami
import org.kde.mauikit 1.2 as Maui
import org.maui.cask 1.0 as Cask
import org.cask.env 1.0 as Env

import "items"
import "items/calendar"


Cask.PanelSection
{
    id: control
   position : ToolBar.Header

    Layout.fillWidth: true

    preferredHeight: win.formFactor === Env.Env.Phone ?  Maui.Style.rowHeight: Maui.Style.toolBarHeightAlt
    margins: win.formFactor === Env.Env.Desktop ? Maui.Style.space.medium : 0
    radius: win.formFactor === Env.Env.Desktop ? Maui.Style.radiusV : 0

    middleContent: CalendarItem
    {
        onClicked: _statusBar.open(card.index)
    }

    leftContent: [
        Cask.PanelItem
        {
            visible: !isMobile
            icon.name: "camera-ready"
        },

        Cask.PanelItem
        {
            id: _micItem
            visible: !isMobile
            icon.name: "mic-ready"
            onClicked: _statusBar.open(card.index)

            card: Cask.PanelCard
            {
                implicitHeight: _tooglesGrid.contentHeight + Maui.Style.space.big
                alignment: _micItem.parent.alignment
                Maui.GridView
                {
                    id:_tooglesGrid
                    width: parent.width
                    height: _tooglesGrid.contentHeight
                    anchors.centerIn: parent
                    itemSize: 64
                    verticalScrollBarPolicy:  Qt.ScrollBarAlwaysOff

                    model: ["network-bluetooth", "input-keyboard-virtual", "rotation-allowed","webcam", "accessories-calculator",  "settings-configure"]
                    delegate: ItemDelegate
                    {
                        height: _tooglesGrid.cellHeight
                        width: _tooglesGrid.cellWidth
                        background: null
                        ToolButton
                        {
                            icon.name: modelData

                            anchors.fill: parent
                            icon.width: Maui.Style.iconSizes.medium
                            icon.height:  Maui.Style.iconSizes.medium
                        }
                    }
                }

            }
        },

        Cask.PanelItem
        {
            id: _playbackItem
            visible: !isMobile
            icon.name: "headphones"
            onClicked: _statusBar.open(card.index)

            card: Cask.PanelCard
            {
                alignment: _playbackItem.parent.alignment
                padding: 0

                Maui.ListItemTemplate
                {
                    width: parent.width
                    height: 100

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
        },

        Cask.PanelItem
        {
            id: _volumeItem
            visible: !isMobile
            icon.name: "audio-volume-medium"
            onClicked: _statusBar.open(card.index)

            card: Cask.PanelCard
            {
                alignment: _volumeItem.parent.alignment

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

    ]

    rightContent:[

        Cask.PanelItem
        {
            id: _powerItem
            display: ToolButton.TextBesideIcon
            icon.name: "battery-080"
            text: "80%"
            onClicked: _statusBar.open(card.index)

            card: Cask.PanelCard
            {
                 alignment: _powerItem.parent.alignment

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
        },

        NetworkItem
        {
            onClicked: _statusBar.open(card.index)
        },

        Cask.PanelItem
        {
            id: _notificationsItem
            icon.name: "notifications"
            onClicked: _statusBar.open(card.index)

            card: Cask.PanelCard
            {
                alignment: _notificationsItem.parent.alignment
                padding: 0
                title: _nof.count + " " + qsTr("Notifications")
                headBar.visible: true
                headBar.leftContent: ToolButton
                {
                    icon.name: "configure"
                }

                headBar.rightContent: ToolButton
                {
                    icon.name: "edit-clear-all"
                }

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
        } ]

}
