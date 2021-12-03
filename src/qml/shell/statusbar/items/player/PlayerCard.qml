import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3

import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.3 as Maui
import org.maui.cask 1.0 as Cask
import org.kde.plasma.core 2.0 as PlasmaCore
import QtGraphicalEffects 1.0

Pane
{
    id: control
    implicitHeight: _template.implicitHeight + topPadding+bottomPadding

    property string source
    property string sourceName
    property string playerName
    property string playerIcon
    property var sourceData
    property var currentMetadata : sourceData ? sourceData.Metadata : undefined

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
    property string artist: {
        if (!currentMetadata) {
            return ""
        }
        var xesamArtist = currentMetadata["xesam:artist"]
        if (!xesamArtist) {
            return "";
        }

        if (typeof xesamArtist == "string") {
            return xesamArtist
        } else {
            return xesamArtist.join(", ")
        }
    }

    property int position: (control.sourceData && control.sourceData.Position) || 0
    readonly property real rate: (control.sourceData && control.sourceData.Rate) || 1
    readonly property int length: currentMetadata ? currentMetadata["mpris:length"] || 0 : 0
    readonly property bool canSeek: (control.sourceData && control.sourceData.CanSeek) || false
    property bool noPlayer: mpris2Source.sources.length <= 1
    readonly property string identity: !control.noPlayer ? sourceData.Identity || source : ""

    readonly property bool canControl: (!control.noPlayer && sourceData.CanControl) || false
    readonly property bool canGoPrevious: (canControl && sourceData.CanGoPrevious) || false
    readonly property bool canGoNext: (canControl && sourceData.CanGoNext) || false
    readonly property bool canPlay: (canControl && sourceData.CanPlay) || false
    readonly property bool canPause: (canControl && sourceData.CanPause) || false

    // var instead of bool so we can use "undefined" for "shuffle not supported"
    readonly property var shuffle: !control.noPlayer && typeof sourceData.Shuffle === "boolean" ? sourceData.Shuffle : undefined
    readonly property var loopStatus: !control.noPlayer && typeof sourceData.LoopStatus === "string" ? sourceData.LoopStatus : undefined
    property bool disablePositionUpdate: false

    //    Component.onCompleted: retrievePosition()

    //    onPositionChanged:
    //    {
    //        console.log("UPDATING POSITION")
    //        // we don't want to interrupt the user dragging the slider
    //        if (!seekSlider.pressed)
    //        {
    //            // we also don't want passive position updates
    //            disablePositionUpdate = true
    //            seekSlider.value = position
    //            disablePositionUpdate = false
    //        }
    //    }

    //    onLengthChanged: {
    //           disablePositionUpdate = true
    //           // When reducing maximumValue, value is clamped to it, however
    //           // when increasing it again it gets its old value back.
    //           // To keep us from seeking to the end of the track when moving
    //           // to a new track, we'll reset the value to zero and ask for the position again
    //           seekSlider.value = 0
    //           seekSlider.to = control.length
    ////           retrievePosition()
    //           disablePositionUpdate = false
    //       }



    background: Rectangle
    {
        radius: 10
        color: Kirigami.Theme.backgroundColor

        //        layer.enabled: true
        //        layer.effect: HueSaturation {
        //            cached: true

        //            lightness: -0.5
        //            saturation: 0.9

        //            layer.enabled: true
        //            layer.effect: GaussianBlur {
        //                cached: true
        //                source: _template.iconItem
        //                radius: 128
        //                deviation: 12
        //                samples: 63

        //                transparentBorder: false
        //            }
        //        }
    }

    contentItem: ColumnLayout
    {
        id: _template
        spacing: Maui.Style.space.medium

        RowLayout
        {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignRight
            Kirigami.Icon
            {
                implicitHeight: 16
                implicitWidth: 16
                source: control.playerIcon
            }

            Label
            {
                Layout.fillWidth: true
                text: control.playerName
            }

            ToolButton
            {
                flat: true
                icon.name: "quickopen"
                onClicked: control.action_open()
            }

            ToolButton
            {
                flat: true
                icon.name: "window-close"
                onClicked: control.action_quit()
            }
        }

        RowLayout
        {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: Maui.Style.space.big

            Item
            {
                implicitHeight: 100
                implicitWidth: implicitHeight

                Maui.IconItem
                {
                    anchors.fill: parent
                    imageSource: control.albumArt
                    iconSource: control.playerIcon
                    iconSizeHint: 48
                    maskRadius: 10
                    imageSizeHint: 80
                }
            }

            ColumnLayout
            {
                implicitHeight: 120
                Layout.fillHeight: true
                Layout.fillWidth: true

                Label
                {
                    visible: text.length
                    Layout.fillWidth: true
                    //                    Layout.fillHeight: true
                    text: control.track
                    wrapMode: Text.Wrap
                    font.pointSize: Maui.Style.fontSizes.big
                    font.bold: true
                }

                Label
                {
                    visible: text.length
                    Layout.fillWidth: true
                    //                    Layout.fillHeight: true

                    wrapMode: Text.NoWrap
                    text: control.artist
                }

                Row
                {
                    spacing: Maui.Style.space.medium
                    ToolButton
                    {
                        icon.name: "media-skip-backward"
                        enabled: control.canGoPrevious
                        onClicked:
                        {
                            seekSlider.value = 0    // Let the media start from beginning. Bug 362473
                            control.action_previous()
                        }

                    }

                    ToolButton
                    {
                        //                        text: qsTr("Play and pause")
                        enabled: control.state == "playing" ? control.canPause : control.canPlay
                        icon.name: control.state == "playing" ? "media-playback-pause" : "media-playback-start"
                        onClicked: control.togglePlaying()

                    }

                    ToolButton
                    {
                        icon.name: "media-skip-forward"
                        enabled: control.canGoNext
                        onClicked:
                        {
                            seekSlider.value = 0    // Let the media start from beginning. Bug 362473
                            control.action_next()
                        }

                    }
                }
            }

        }

        Slider
        {
            id: seekSlider
            Layout.fillWidth: true
            //            value: 50000000
            from: 0
            value: control.position/control.length
            to: 1
            visible: canSeek
            onMoved:
            {
                var service = mpris2Source.serviceForSource(control.sourceName)
                var operation = service.operationDescription("SetPosition")
                operation.microseconds = seekSlider.value*control.length
                service.startOperationCall(operation)

            }

            //            Timer
            //            {
            //                id: seekTimer
            //                interval: 1000
            //                repeat: true
            //                running: control.state === "playing" && interval > 0 && seekSlider.to >= 1000000
            //                onTriggered: {
            //                    // some players don't continuously update the seek slider position via mpris
            //                    // add one second; value in microseconds
            //                    console.log("UPDATE SLIDER")
            //                    if (!seekSlider.pressed)
            //                    {
            //                        disablePositionUpdate = true
            //                        if (seekSlider.value == seekSlider.to) {
            //                            retrievePosition()
            //                        } else {
            //                            seekSlider.value += 1000000
            //                        }
            //                        disablePositionUpdate = false
            //                    }
            //                }
            //            }

        }

    }

    function togglePlaying() {
        if (control.state === "playing") {
            if (control.canPause) {
                control.action_pause();
            }
        } else {
            if (control.canPlay) {
                control.action_play();
            }
        }
    }

    function action_open() {
        serviceOp(control.sourceName, "Raise");
    }

    function action_quit() {
        serviceOp(control.sourceName, "Quit");
    }

    function action_play() {
        serviceOp(control.sourceName, "Play");
    }

    function action_pause() {
        serviceOp(control.sourceName, "Pause");
    }

    function action_playPause() {
        serviceOp(control.sourceName, "PlayPause");
    }

    function action_previous() {
        serviceOp(control.sourceName, "Previous");
    }

    function action_next() {
        serviceOp(control.sourceName, "Next");
    }

    function action_stop() {
        serviceOp(control.sourceName, "Stop");
    }

    function serviceOp(src, op) {
        var service = mpris2Source.serviceForSource(src);
        var operation = service.operationDescription(op);
        return service.startOperationCall(operation);
    }

    function retrievePosition() {
        var service = mpris2Source.serviceForSource(control.sourceName);
        var operation = service.operationDescription("GetPosition");
        service.startOperationCall(operation);
    }


    states: [
        State {
            name: "playing"
            when: !control.noPlayer && sourceData.PlaybackStatus === "Playing"

        },
        State {
            name: "paused"
            when: !control.noPlayer && sourceData.PlaybackStatus === "Paused"
        }
    ]

    Component.onDestruction:
    {
        console.log("DESTROY PLAYER CARD")
    }


}

