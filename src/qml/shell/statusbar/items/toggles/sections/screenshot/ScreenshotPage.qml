import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.mauikit.controls as Maui

import org.maui.cask as Cask

import "../../../../../templates"

StackPage
{
    id: control

    implicitHeight: Math.max(200, Math.min(200, _layout.implicitHeight + topPadding + bottomPadding + headerContainer.implicitHeight + footerContainer.implicitHeight + Maui.Style.space.big))

    title: i18n("Screenshot")
    property int display: ToolButton.TextUnderIcon

    Column
    {
        id: _layout
        enabled: !_timer.running

        spacing: Maui.Style.defaultSpacing
        width: control.width
        anchors.centerIn: parent

        RowLayout
        {
            width: parent.width
            spacing: Maui.Style.space.medium

            CardButton
            {
                text: i18n("Screen")
                icon.name: "view-fullscreen"
                display: control.display
                Layout.fillHeight: true
                Layout.fillWidth: true
                onClicked:
                {
                    console.log("take screenshot")
                    control.startScreenshot("screen")
                }
            }

            CardButton
            {
                icon.name: "window"
                text: i18n("Window")
                display: control.display

                Layout.fillHeight: true
                Layout.fillWidth: true

                onClicked:
                {
                    console.log("take screenshot")
                    control.startScreenshot("window")
                }
            }

            CardButton
            {
                icon.name: "select-rectangular"
                text: i18n("Area")
                display: control.display

                Layout.fillHeight: true
                Layout.fillWidth: true

                onClicked:
                {
                    console.log("take screenshot")
                    control.startScreenshot("area")
                }
            }
        }

        Maui.SectionItem
        {
            width: parent.width
            label1.text: "Timer"
            label2.text: i18n("Seconds")
            flat: false
            SpinBox
            {
                id: _secondsSpinBox
                from: 0
                to : 100
                //              value: 0
            }
        }

        Maui.Chip
        {
            id: _counterChip
            visible: _timer.running
            width: parent.width
            property int count  : _secondsSpinBox.value
            text: count + " seconds."

            color: Maui.Theme.positiveTextColor

            Timer
            {
                id: _counter
                interval: 1000
                repeat: true
                //            running: _timer.running
                onTriggered:
                {
                    _counterChip.count--;
                    if(_counterChip.count === 0)
                    {
                        stop()
                        _counterChip.reset()
                    }
                }
            }

            function reset()
            {
                _counterChip.count = Qt.binding(function () {return _secondsSpinBox.value})
            }
        }


    }

    Timer
    {
        id: _timer
        property string type
        interval: _secondsSpinBox.value * 1000
        repeat: false
        onTriggered:
        {
            takeScreenshot(type)
        }
    }

    function startScreenshot(type)
    {
        _timer.type = type
        _timer.restart()
        _counter.restart()
    }

    function takeScreenshot(type)
    {
        switch(type)
        {
        case "screen":  Cask.Server.screenshot.grabCurrentScreen();
            break;
        case "window":  Cask.Server.screenshot.grabCurrentWindow();
            break;
        case "area": break
        default: break;
        }
    }
}
