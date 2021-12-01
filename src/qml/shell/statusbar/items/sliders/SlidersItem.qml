import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5
import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.2 as Maui
import org.maui.cask 1.0 as Cask

Cask.PanelItem
{
    id: control
    iconSize: 16

    Row
    {
        spacing: Maui.Style.space.medium
        Kirigami.Icon
        {
            source: "audio-volume-medium"
            height: control.iconSize
            width: height
            color: control.hovered || control.down || control.pressed ? Kirigami.Theme.highlightColor : Kirigami.Theme.textColor
        }

        Kirigami.Icon
        {
            source: "mic-ready"
            height: control.iconSize
            width: height
            color: control.hovered || control.down || control.pressed ? Kirigami.Theme.highlightColor : Kirigami.Theme.textColor
        }

        Kirigami.Icon
        {
            source: "high-brightness"
            height: control.iconSize
            width: height
            color: control.hovered || control.down || control.pressed ? Kirigami.Theme.highlightColor : Kirigami.Theme.textColor
        }
    }

    card: Cask.PanelCard
    {
        width: ListView.view.width

        Column
        {
            width: parent.width
            spacing: Maui.Style.space.big

            SliderTemplate
            {
                width: parent.width
                iconSource: "high-brightness"
            }


            SliderTemplate
            {
                width: parent.width
                iconSource: "player-volume"
            }

            SliderTemplate
            {
                width: parent.width
                iconSource: "microphone"
            }


        }


    }
}
