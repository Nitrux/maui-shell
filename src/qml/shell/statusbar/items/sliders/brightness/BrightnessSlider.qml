import QtQuick 2.15
import QtQuick.Controls 2.15

import QtQuick.Layouts 1.3

import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.3 as Maui

import org.maui.cask 1.0 as Cask
import org.cask.power 1.0 as CaskPower
import QtQuick.Templates 2.15 as T
import ".."

import "../../../../templates" as Template

Template.SliderToggle
{
    id: control
    visible: _brightnessControl.screenBrightnessAvailable

    CaskPower.BrightnessControl
    {
        id: _brightnessControl
    }

    slider.from: 0
    slider.to: _brightnessControl.maximumScreenBrightness
    slider.stepSize: 1

    Binding
    {
        delayed: true
        target: control.slider
        property : "value"
        value: _brightnessControl.screenBrightness
    }

    slider.iconSource: "high-brightness"


    Connections
    {
        target: control.slider

//        function onMoved()
//        {
//            console.log(control.slider.value)
//        }

        function onPressedChanged()
        {
            if (!control.slider.pressed)
            {
                _brightnessControl.changeScreenBrightness(control.slider.value, true)
            }
        }
    }

    page: BrightnessPage
    {

    }
}
