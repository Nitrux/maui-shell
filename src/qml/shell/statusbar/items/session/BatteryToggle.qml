import QtQuick 2.15
import QtQuick.Controls 2.15

import QtQuick.Layouts 1.3

import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.3 as Maui

import org.maui.cask 1.0 as Cask
import org.cask.power 1.0 as CaskPower

import QtQuick.Templates 2.15 as T

import "../sliders"

BatteryBar
{
    id: control
    visible: _batteryInfo.hasBatteries
    battery: primaryBattery
    label.text: battery ? battery.product + " " + battery.vendor + " / " + battery.type : _batteryInfo.acPluggedIn
    progressbar.enabled: primaryBattery

    iconSource: "preferences-system-power-management"

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
}
