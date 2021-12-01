import QtQuick 2.15
import QtQuick.Controls 2.5

import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.2 as Maui
import org.maui.cask 1.0 as Cask
import org.kde.plasma.core 2.0 as PlasmaCore
import QtGraphicalEffects 1.0

Pane
{
    id: control
    property string source
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

    Maui.ListItemTemplate
    {
        id: _template
        anchors.fill: parent

        imageSource: control.albumArt
        iconSource: control.playerIcon
        iconSizeHint: imageSizeHint
        maskRadius: 10
        imageSizeHint: 90
        headerSizeHint: 90
        spacing: Maui.Style.space.medium

        label1.text: control.track
        label2.text: control.artist

        leftLabels.data: Label
        {
            text: control.playerName
        }

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
        }
    }
}

