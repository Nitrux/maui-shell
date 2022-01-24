import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3

import QtQuick.Templates 2.15 as T
import org.cask.power 1.0 as CaskPower
import org.kde.kirigami 2.14 as Kirigami

import "../../../templates"

AbstractBarToggle
{
    id: control
    property alias progressbar : _bar
    property CaskPower.Battery battery
    property alias iconSource : _icon.source

    bar: T.ProgressBar
    {
        id: _bar
        Layout.fillWidth: true
        implicitHeight: 22

        from: 1
        to : 100
        value : control.battery.percent


        background: Rectangle
        {
            x: _bar.leftPadding
            y: _bar.topPadding + _bar.availableHeight / 2 - height / 2
            implicitWidth: 200
            implicitHeight: 4
            width: _bar.availableWidth
            height: implicitHeight
            radius: height/2
            color: Qt.darker(Kirigami.Theme.backgroundColor, 1.2)
        }

        contentItem: Item
        {
            implicitWidth: _bar.height
            implicitHeight: _bar.height

            Rectangle
            {
                height: parent.height

                width: _bar.visualPosition * parent.width
                color: control.batteryColor(control.battery.stateName)
                radius: height/2
            }

            Kirigami.Icon
            {
                id: _icon
                height: 16
                width : 16
                color: Kirigami.Theme.highlightedTextColor
                anchors.centerIn: parent
            }
        }
    }


    function batteryColor(state)
    {
        switch(state)
        {
        case "NoCharge" : return Kirigami.Theme.negativeBackgroundColor;
        case "Charging" : return Kirigami.Theme.positiveBackgroundColor;
        case "Discharging" : return Kirigami.Theme.neutralBackgroundColor;
        case "FullyCharged" : return Kirigami.Theme.highlightColor;

        }
    }
}
