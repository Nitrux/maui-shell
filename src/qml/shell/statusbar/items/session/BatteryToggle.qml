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
}
