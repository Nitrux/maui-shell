import QtQuick 2.15
import QtQuick.Controls 2.15

import QtQuick.Layouts 1.3

import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.3 as Maui

import org.maui.cask 1.0 as Cask
import org.cask.power 1.0 as CaskPower
import QtQuick.Templates 2.15 as T
import ".."

SliderToggle
{
    id: control
    CaskPower.BrightnessControl
    {
        id: _brightnessControl
    }
    slider.from: 0
    slider.to: _brightnessControl.maximumScreenBrightness
    slider.value: _brightnessControl.screenBrightness
    slider.iconSource: "high-brightness"
    page: Page{

    }
}
