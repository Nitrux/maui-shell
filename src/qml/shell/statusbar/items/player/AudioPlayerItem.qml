import QtQuick 2.15
import QtQuick.Controls 2.5

import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.2 as Maui
import org.maui.cask 1.0 as Cask


Cask.PanelItem
{
    id: control
    property var mprisSourcesModel: []
readonly property bool isPlaying : _playersList.currentItem.item.isPlaying
    Row
    {
        spacing: control.spacing
        Kirigami.Icon
        {
            source: "headphones"
            height: control.iconSize
            width: height
            color: control.icon.color
        }
        Kirigami.Icon
        {
            source: isPlaying ? "media-playback-pause" : "media-playback-start"

            height: control.iconSize
            width: height
            color: control.icon.color
        }
    }

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
        //        padding: Maui.Style.space.big

        //        bottomPadding :22

        ListView
        {
            id: _playersList

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
            width: parent.width
            implicitHeight: Math.max(200, holder.implicitHeight)
            spacing: Maui.Style.space.medium

            orientation: ListView.Horizontal
            snapMode: ListView.SnapOneItem

            boundsBehavior: Flickable.StopAtBounds
            boundsMovement :Flickable.StopAtBounds

            //            interactive: Kirigami.Settings.hasTransientTouchInput
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
                }
            }
        }

        Item
        {
            height: 22
            width: parent.width
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
