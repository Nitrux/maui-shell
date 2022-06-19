import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3

import org.mauikit.controls 1.2 as Maui
import org.maui.cask 1.0 as Cask

import QtQuick.Templates 2.15 as T

Cask.PanelItem
{
    id: control
    property var mprisSourcesModel: []
    readonly property bool isPlaying : _playersList.currentItem.item.isPlaying


    Cask.IconLabel
    {
        icon.source: isPlaying ? "media-playback-start" : "media-playback-pause"
        height: control.iconSize
        icon.color: control.icon.color
    }

    Cask.IconLabel
    {
        visible: labelsVisible
        text: _playersList.currentItem.item.track
        width: Math.min(implicitWidth, 200)
        icon.source: "headphones"
        height: control.iconSize
        icon.color: control.icon.color
    }

//    Maui.ListItemTemplate
//    {
//        visible:  labelsVisible
//        width: 200
//        label1.text: _playersList.currentItem.item.track
//        imageSource: _playersList.currentItem.item.albumArt
//        maskRadius: 6
//        headerSizeHint: 32
//    }

    Cask.Mpris
    {
        id: mpris
    }

    //    Component.onCompleted: {
    //        mpris2Source.serviceForSource("@multiplex").enableGlobalShortcuts()
    //        updateMprisSourcesModel()
    //    }


    //    function updateMprisSourcesModel () {

    //        var model = [{
    //                         'text': i18n("Choose player automatically"),
    //                         'icon': 'emblem-favorite',
    //                         'source': mpris2Source.multiplexSource
    //                     }]

    //        var sources = mpris2Source.sources
    //        for (var i = 0, length = sources.length; i < length; ++i) {
    //            var source = sources[i]
    //            if (source === mpris2Source.multiplexSource) {
    //                continue
    //            }

    //            const playerData = mpris2Source.data[source];
    //            // source data is removed before its name is removed from the list
    //            if (!playerData) {
    //                continue;
    //            }

    //            model.push({
    //                           'text': playerData["Identity"],
    //                           'icon': playerData["Desktop Icon Name"] || playerData["DesktopEntry"] || "emblem-music-symbolic",
    //                           'source': source
    //                       });
    //        }

    //        control.mprisSourcesModel = model;
    //    }

    card: Cask.PanelCard
    {
        width: ListView.view.width
        onClosed: _togglesStack.pop()

        T.StackView
        {
            id : _togglesStack
            width: parent.width
            height: currentItem.implicitHeight
            clip: true
            padding: 0

            initialItem: ColumnLayout
            {

                ListView
                {
                    id: _playersList
                    implicitHeight: Math.max(150, holder.implicitHeight)
                    Layout.fillWidth: true

                    Maui.Holder
                    {
                        id:  holder
                        anchors.fill: parent
                        visible: _playersList.count === 0
                        emoji: "music-note-16th"
                        title: i18n("No Players")
                        body: i18n("Launch a new player to control")
                        Action
                        {
                            text: "Vvave"
                            onTriggered: dock.launchExec("vvave")
                        }
                    }

                    //            currentIndex: pageIndicator.currentIndex
                    spacing: Maui.Style.space.medium

                    orientation: ListView.Horizontal
                    snapMode: ListView.SnapOneItem

                    boundsBehavior: Flickable.StopAtBounds
                    boundsMovement :Flickable.StopAtBounds

                    highlightFollowsCurrentItem: true
                    highlightMoveDuration: 0
                    highlightResizeDuration : 0
                    highlightRangeMode:ListView.ApplyRange
                    keyNavigationEnabled: true
                    keyNavigationWraps : true

                    onMovementEnded:
                    {
                        currentIndex = indexAt(contentX, contentY)
                    }

                    clip: true
                    model: mpris.players

                    delegate: Loader
                    {
                        width: ListView.view.width
                        height: ListView.view.height
                        asynchronous: true
                        active: ListView.isCurrentItem

                        ListView.onAdd: ListView.view.incrementCurrentIndex()
                        ListView.onRemove: ListView.view.decrementCurrentIndex()

                        sourceComponent: PlayerCard
                        {
                            player:  model.player
                            onClicked: _togglesStack.push(page)
                        }
                    }
                }

                Item
                {
                    implicitHeight: 22
                    Layout.fillWidth: true
                    visible: _playersList.count > 1
                    PageIndicator
                    {
                        id: pageIndicator
                        anchors.centerIn: parent

                        count: _playersList.count
                        currentIndex: _playersList.currentIndex
                    }
                }
            }
        }
    }
}
