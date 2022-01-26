import QtQuick 2.15
import QtQuick.Controls 2.15

import QtQuick.Layouts 1.3

import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.3 as Maui

import org.maui.cask 1.0 as Cask
import QtQuick.Templates 2.15 as T

//import org.cask.audio 1.0 as CaskAudio
import org.kde.plasma.private.volume 0.1 as PA

import ".."

import "../../../../templates" as Template

Template.SliderToggle
{
    id: control
    property string /* "sink" | "sink-input" | "source" | "source-output" */ type
    readonly property var currentPort: model.Ports[model.ActivePortIndex]
    readonly property bool muted: model.Muted
    readonly property int activePortIndex: model.ActivePortIndex
readonly property int percent: (control.volume * 100) / PA.PulseAudio.NormalVolume
    readonly property bool isPlayback: type.startsWith("sink")
    slider.iconSource: "audio-volume-high"

    property int volume: model.Volume
    property bool ignoreValueChange: true

    slider.from: PA.PulseAudio.MinimalVolume
    slider.to: PA.PulseAudio.MaximalVolume
    slider.stepSize: slider.to / (slider.to / PA.PulseAudio.NormalVolume * 100.0)
    visible: model.HasVolume
    enabled: model.VolumeWritable
    slider.opacity: model.Muted ? 0.5 : 1
    //slider.value: 0.5
    slider.animatedRec.width: (slider.handle.x * meter.volume) + Maui.Style.space.medium

    data: [PA.VolumeMonitor
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
