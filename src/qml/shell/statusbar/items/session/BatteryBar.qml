import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.mauicore.power as CaskPower
import org.mauikit.controls as Maui

import "../../../templates"

AbstractBarToggle
{
    id: control
    property alias progressbar : _bar
    property CaskPower.Battery battery
    property alias iconSource : _bar.iconSource

    bar: ProgressBarTemplate
    {
        id: _bar
        Layout.fillWidth: true
        color: control.batteryColor(control.battery ? control.battery.stateName : "")

        from: 0
        to : 100
        value : control.battery ? control.battery.percent : 0
    }

    function batteryColor(state)
    {
        switch(state)
        {
        case "NoCharge" : return Maui.Theme.negativeBackgroundColor;
        case "Charging" : return Maui.Theme.positiveBackgroundColor;
        case "Discharging" : return Maui.Theme.neutralBackgroundColor;
        case "FullyCharged" : return Maui.Theme.highlightColor;
        default: return Maui.Theme.textColor;
        }
    }
}
