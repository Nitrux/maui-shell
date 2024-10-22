import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.mauikit.controls as Maui
import org.maui.cask as Cask

ItemDelegate
{
    id: control
    padding: 0

    property Cask.MprisPlayer player
    property Component page: PlayerPage
    {
        player: control.player
    }

    readonly property bool isPlaying : state === "Playing"
    property string state : player ? player.status : ""

    readonly property var currentMetadata : player ? player.metadata : undefined

    property string albumArt: currentMetadata ? currentMetadata["mpris:artUrl"] || "" : ""

    property string track: {
        if (!currentMetadata) {
            return ""
        }
        var xesamTitle = currentMetadata["xesam:title"]
        if (xesamTitle) {
            return xesamTitle
        }
        // if no track title is given, print out the file name
        var xesamUrl = currentMetadata["xesam:url"] ? currentMetadata["xesam:url"].toString() : ""
        if (!xesamUrl) {
            return ""
        }
        var lastSlashPos = xesamUrl.lastIndexOf('/')
        if (lastSlashPos < 0) {
            return ""
        }
        var lastUrlPart = xesamUrl.substring(lastSlashPos + 1)
        return decodeURIComponent(lastUrlPart)
    }

    readonly property bool canControl: (control.player && control.player.capabilities & Cask.MprisPlayer.CanControl) || false

    readonly property bool canGoPrevious: (control.player && control.player.capabilities & Cask.MprisPlayer.CanGoPrevious) || false

    readonly property bool canGoNext:(control.player && control.player.capabilities & Cask.MprisPlayer.CanGoNext) || false

    readonly property bool canPlay: control.player ? (control.player.capabilities & Cask.MprisPlayer.CanPlay) : false

    readonly property bool canPause: control.player ? (control.player.capabilities & Cask.MprisPlayer.CanPause) : false

    background: null

    contentItem: Item
    {
        Layout.fillWidth: true
        Layout.fillHeight: true

        Maui.IconItem
        {
            anchors.fill: parent
            fillMode: Image.PreserveAspectCrop
            imageSource: control.albumArt
            iconSource: control.playerIcon
            maskRadius: Maui.Style.radiusV

            Rectangle
            {
                anchors.fill: parent

                gradient: Gradient
                {
                    GradientStop { position: 0.0; color: "transparent" }
                    GradientStop { position: 1.2-Math.min(1, _labels.implicitHeight/height);  color: control.Maui.Theme.backgroundColor }
                }
            }

            RowLayout
            {
                id: _labels
                anchors.bottom: parent.bottom
                width: parent.width
                spacing: Maui.Style.space.big

                ColumnLayout
                {
                    Layout.fillWidth: true

                    Label
                    {
                        visible: text.length
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignCenter
                        horizontalAlignment: Qt.AlignHCenter
                        verticalAlignment: Qt.AlignVCenter
                        text: control.track
                        wrapMode: Text.Wrap
                        font.pointSize: Maui.Style.fontSizes.big
                        font.bold: true
                    }

                    Row
                    {
                        spacing: Maui.Style.space.medium
                        Layout.alignment: Qt.AlignHCenter
                        ToolButton
                        {
                            icon.name: "media-skip-backward"
                            enabled: control.canGoPrevious
                            onClicked:
                            {
                                control.action_previous()
                            }

                        }

                        ToolButton
                        {
                            enabled: control.canPlay
                            icon.name: control.isPlaying ? "media-playback-pause" : "media-playback-start"
                            onClicked: control.togglePlaying()

                        }

                        ToolButton
                        {
                            icon.name: "media-skip-forward"
                            enabled: control.canGoNext
                            onClicked:
                            {
                                control.action_next()
                            }

                        }
                    }
                }
            }
        }
    }

    function togglePlaying() {
        if (control.isPlaying) {
            if (control.canPause) {
                control.action_pause();
            }
        } else {
            if (control.canPlay) {
                control.action_play();
            }
        }
    }

    function action_play() {
        control.player.play()
    }

    function action_pause() {
        control.player.pause()
    }

    function action_playPause() {
        control.player.playPause()
    }

    function action_previous() {
        control.player.previous()
    }

    function action_next() {
        control.player.next()
    }

    function action_stop() {
        control.player.stop()
    }

    Component.onDestruction:
    {
        console.log("DESTROY PLAYER CARD")
    }

}

