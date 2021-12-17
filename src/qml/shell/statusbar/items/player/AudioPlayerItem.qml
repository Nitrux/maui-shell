import QtQuick 2.15
import QtQuick.Controls 2.5

import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.2 as Maui
import org.maui.cask 1.0 as Cask


Cask.PanelItem
{
    id: control
    property var mprisSourcesModel: []

    Row
    {
        spacing: Maui.Style.space.medium
        Kirigami.Icon
        {
            source: "headphones"
            height: control.iconSize
            width: height
            color: control.icon.color
        }
        Kirigami.Icon
        {
            source: _playersList.currentItem.item.state == "playing" ? "media-playback-pause" : "media-playback-start"

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
        Kirigami.Theme.inherit: false
        Kirigami.Theme.colorSet: Kirigami.Theme.Complementary



        ListView
        {
            id: _playersList

            Maui.Holder
            {
                anchors.fill: parent
                visible: _playersList.count === 0
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
            implicitHeight: 200 + pageIndicator.height
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
                height: 200
                asynchronous: true
                active: ListView.isCurrentItem

                sourceComponent: PlayerCard
                {
//                    color: "red"
                    player:  model.player
                }
            }

            PageIndicator
            {
                id: pageIndicator
                //                interactive: true
                count: _playersList.count
                currentIndex: _playersList.currentIndex

                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }
}
