import QtQuick 2.15
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5
import QtQuick.Templates 2.15 as T

import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.2 as Maui
import org.maui.cask 1.0 as Cask

import "volume"
import "brightness"

Cask.PanelItem
{
    id: control
    property alias micItem : _micSlider

    Row
    {
        spacing: control.spacing
        Kirigami.Icon
        {
            source: _audioSlider.slider.iconSource
            height: control.iconSize
            width: height
            color: control.icon.color
        }

        Kirigami.Icon
        {
            source: _micSlider.slider.iconSource
            height: control.iconSize
            width: height
            color: control.icon.color
        }

        Kirigami.Icon
        {
            source: "high-brightness"
            height: control.iconSize
            width: height
            color: control.icon.color
        }
    }

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

            initialItem: Column
            {
                spacing: control.card.spacing

                VolumeSlider
                {
                    id: _audioSlider
                    width: parent.width
                    onClicked: _togglesStack.push(page)
                }

                MicSlider
                {
                    id: _micSlider
                    width: parent.width
                    onClicked: _togglesStack.push(page)
                }

                BrightnessSlider
                {
                    width: parent.width
                    onClicked: _togglesStack.push(page)
                }


            }
        }
    }
}
