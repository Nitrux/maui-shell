import QtQuick
import QtQuick.Controls

import QtQuick.Layouts

import org.mauikit.controls as Maui

import org.mauicore.audio as MauiCore

import ".."
import "../../../../templates" as Template

Template.SliderToggle
{
    id: control
    
    property string /* "sink" | "sink-input" | "source" | "source-output" */ type
   
    readonly property var currentPort: model.Ports[model.ActivePortIndex]
    readonly property bool muted: model.Muted
    readonly property int activePortIndex: model.ActivePortIndex
    readonly property int percent: (control.volume * 100) / MauiCore.PulseAudio.NormalVolume
    readonly property bool isPlayback: type.startsWith("sink")

    readonly property int volume: model.Volume
    property bool ignoreValueChange: true

    slider.from: MauiCore.PulseAudio.MinimalVolume
    slider.to: MauiCore.PulseAudio.MaximalVolume
    slider.stepSize: slider.to / (slider.to / MauiCore.PulseAudio.NormalVolume * 100.0)
    slider.iconSource: "audio-volume-high"

    slider.opacity: model.Muted ? 0.5 : 1
    slider.animatedRec.width: (slider.handle.x * meter.volume) + Maui.Style.space.medium
    
    visible: model.HasVolume
    enabled: model.VolumeWritable

    data: [MauiCore.VolumeMonitor
    {
        id: meter
        target: slider.visible && model.PulseObject ? model.PulseObject : null
    },

    Timer
    {
        id: updateTimer
        interval: 200
        triggeredOnStart: true
        onTriggered: updateSliderValue(control.volume)
    }]

    onVolumeChanged:
    {
        var oldIgnoreValueChange = ignoreValueChange;
        ignoreValueChange = true;
        control.slider.value = model.Volume;
        ignoreValueChange = oldIgnoreValueChange;
    }

    Connections
    {
        target: control.slider
        function onValueChanged()
        {
            if (!ignoreValueChange)
            {
                model.Volume = target.value;
                model.Muted = target.value === 0;

                if (!control.slider.pressed)
                {
                    updateTimer.restart();
                }
            }
        }

        function onPressedChanged()
        {
            if (!control.slider.pressed) {
                // Make sure to sync the volume once the button was
                // released.
                // Otherwise it might be that the slider is at v10
                // whereas PA rejected the volume change and is
                // still at v15 (e.g.).
                updateTimer.restart();

                if (control.type === "sink") {
                    playFeedback(model.Index);
                }
            }
        }
    }

    label.text:
    {
        if (currentPort && currentPort.description) {
            if (ListView.view.count === 1 || !model.Description) {
                return currentPort.description;
            } else {
                return i18nc("label of device items", "%1 (%2)", currentPort.description, model.Description);
            }
        }
        if (model.Description) {
            return model.Description;
        }
        if (model.Name) {
            return model.Name;
        }
        return i18n("Device name not found");
    }


    function updateSliderValue(value)
    {
        slider.value = value
    }

    Component.onCompleted:
    {
        ignoreValueChange = false;
        updateTimer.start()
    }

}
