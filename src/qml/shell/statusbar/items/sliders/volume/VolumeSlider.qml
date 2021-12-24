import QtQuick 2.15
import QtQuick.Controls 2.15

import QtQuick.Layouts 1.3

import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.3 as Maui

import org.maui.cask 1.0 as Cask
import QtQuick.Templates 2.15 as T
import ".."

SliderToggle
{
    id: control

    slider.iconSource:  "player-volume"
    page: VolumePage
    {

    }

}
