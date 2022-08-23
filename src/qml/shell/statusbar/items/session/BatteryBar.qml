import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3

import QtQuick.Templates 2.15 as T
import org.mauicore.power 1.0 as CaskPower

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
        color: control.batteryColor(control.battery.stateName)

        from: 1
        to : 100
        value : control.battery.percent
    }

    function batteryColor(state)
    {
        switch(state)
        {
        case "NoCharge" : return Maui.Theme.negativeBackgroundColor;
        case "Charging" : return Maui.Theme.positiveBackgroundColor;
        case "Discharging" : return Maui.Theme.neutralBackgroundColor;
        case "FullyCharged" : return Maui.Theme.highlightColor;

        }
    }
}
