import QtQuick
import QtQuick.Controls

import QtQuick.Layouts

import org.mauikit.controls as Maui

import org.maui.cask as Cask
import org.mauicore.power as CaskPower

import "../sliders"

BatteryBar
{
    id: control
    visible: _batteryInfo.hasBatteries
    battery: primaryBattery
    label.text: (primaryBattery ? battery.prettyName : i18n("AC Powered"))
    label2.text: formatSecs(battery.pluggedIn ? battery.timeToFull : battery.timeToEmpty)
    progressbar.enabled: primaryBattery
    icon.source: batteryIcon(battery.percent, battery.stateName === "Charging")

    iconSource: icon.source

    page: PowerPage
    {
        model: _batteryInfo.batteries
    }

    property alias primaryBattery : _batteryInfo.primaryBattery

    CaskPower.BatteryInfo
    {
        id: _batteryInfo
    }

    function batteryIcon(p, charging)
    {
        let stateName = charging ? "-charging" : "";

        if (p >= 95) {
            return "battery-100" + stateName;
        } else if (p >= 85) {
            return "battery-090" + stateName;
        } else if (p >= 75) {
            return "battery-080" + stateName;
        } else if (p >= 65) {
            return "battery-070" + stateName;
        } else if (p >= 55) {
            return "battery-060" + stateName;
        } else if (p >= 45) {
            return "battery-050" + stateName;
        } else if (p >= 35) {
            return "battery-040" + stateName;
        } else if (p >= 25) {
            return "battery-030" + stateName;
        } else if (p >= 15) {
            return "battery-020" + stateName;
        } else if (p > 5) {
            return "battery-010" + stateName;
        } else {
            return "battery-000" + stateName;
        }
    }

    function formatSecs(secs)
    {
            var sec_num = parseInt(secs, 10)
            var hours   = Math.floor(sec_num / 3600)
            var minutes = Math.floor(sec_num / 60) % 60
            var seconds = sec_num % 60

            return [hours,minutes,seconds]
                .map(v => v < 10 ? "0" + v : v)
                .filter((v,i) => v !== "00" || i > 0)
                .join(":")


    }
}
