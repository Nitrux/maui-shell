import QtQuick
import QtQuick.Controls

import QtQuick.Layouts

import org.mauikit.controls as Maui

import org.maui.cask as Cask
import org.mauicore.power as CaskPower

import ".."
import "../../../../templates" as Template

Template.SliderToggle
{
    id: control
    readonly property alias available : _brightnessControl.screenBrightnessAvailable
    visible: control.available

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
