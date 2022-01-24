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
    property var defaultSinkValue: defaultSink ? defaultSink.volume / PA.PulseAudio.MaximalVolume * 100.0 : -1
    property var defaultSink: paSinkModel.defaultSink
    property bool globalMute: false
    readonly property string dummyOutputName: "auto_null"
    property int wheelDelta: 0
    property int volumePercentStep: 5
    property int currentMaxVolumePercent: 100
       property int currentMaxVolumeValue: currentMaxVolumePercent * PA.PulseAudio.NormalVolume / 100.00

    page: VolumePage
    {

    }

    slider.iconSource: volumeIconName
    slider.from: PA.PulseAudio.MinimalVolume
    slider.to: PA.PulseAudio.MaximalVolume
    slider.stepSize: slider.to / (slider.to / PA.PulseAudio.MaximalVolume * 100.0)
    slider.value: defaultSink ? defaultSink.volume : 0

    onWheel:
    {
        var delta = wheel.angleDelta.y || wheel.angleDelta.x;
        wheelDelta += delta;
        // Magic number 120 for common "one click"
        // See: https://qt-project.org/doc/qt-5/qml-qtquick-wheelevent.html#angleDelta-prop
        while (wheelDelta >= 120) {
            wheelDelta -= 120;
            increaseVolume();
        }
        while (wheelDelta <= -120) {
            wheelDelta += 120;
            decreaseVolume();
        }
    }

    Connections
    {
        target: control.slider
        function onValueChanged()
        {
            if (!defaultSink)
                return

            defaultSink.volume = target.value
            defaultSink.muted = (target.value === 0)
        }

        function onPressedChanged()
        {
            if (!control.slider.pressed)
            {
                playFeedback(paSinkModel.defaultSink.index)
            }
        }
    }

    property var volumeIconName:
    {
        if (defaultSinkValue <= 0)
            return "audio-volume-muted"
        else if (defaultSinkValue <= 25)
            return "audio-volume-low"
        else if (defaultSinkValue <= 75)
            return "audio-volume-medium"
        else
            return "audio-volume-high"
    }

    PA.SinkModel
    {
        id: paSinkModel

        onDefaultSinkChanged:
        {
            if (!defaultSink) {
                return
            }
        }
    }

    // Input devices
    PA.SourceModel
    {
        id: paSourceModel
    }
    // Confusingly, Sink Input is what PulseAudio calls streams that send audio to an output device
    PA.SinkInputModel { id: paSinkInputModel }

    // Confusingly, Source Output is what PulseAudio calls streams that take audio from an input device
    PA.SourceOutputModel { id: paSourceOutputModel }
    PA.PulseObjectFilterModel
    {
        id: paSinkFilterModel
        sortRole: "SortByDefault"
        sortOrder: Qt.DescendingOrder
        //        filterOutInactiveDevices: true
        sourceModel: paSinkModel
    }

    // non-virtual streams going to output devices
    PA.PulseObjectFilterModel {
        id: paSinkInputFilterModel
        filters: [ { role: "VirtualStream", value: false } ]
        sourceModel: paSinkInputModel
    }

    // non-virtual streams coming from input devices
    PA.PulseObjectFilterModel {
        id: paSourceOutputFilterModel
        filters: [ { role: "VirtualStream", value: false } ]
        sourceModel: paSourceOutputModel
    }

    PA.CardModel
    {
        id: paCardModel
    }

    PA.VolumeFeedback
    {
        id: feedback
    }

    function playFeedback(sinkIndex)
    {
        if (sinkIndex == undefined)
        {
            sinkIndex = paSinkModel.preferredSink.index;
        }
        feedback.play(sinkIndex);
    }

    function increaseVolume()
    {
        if (globalMute)
        {
            disableGlobalMute();
        }
        changeSpeakerVolume(volumePercentStep);
    }

    function decreaseVolume()
    {
        if (globalMute)
        {
            disableGlobalMute();
        }
        changeSpeakerVolume(-volumePercentStep);
    }

    function changeSpeakerVolume(deltaPercent)
    {
        if (!paSinkModel.preferredSink || isDummyOutput(paSinkModel.preferredSink))
        {
            return;
        }
        const newPercent = changeVolumeByPercent(paSinkModel.preferredSink, deltaPercent);
        //        osd.showVolume(newPercent);
        playFeedback();
    }

    function changeVolumeByPercent(volumeObject, deltaPercent)
    {
        const oldVolume = volumeObject.volume;
        const oldPercent = volumePercent(oldVolume);
        const targetPercent = oldPercent + deltaPercent;
        const newVolume = boundVolume(Math.round(PA.PulseAudio.NormalVolume * (targetPercent/100)));
        const newPercent = volumePercent(newVolume);
        volumeObject.muted = newPercent == 0;
        volumeObject.volume = newVolume;
        return newPercent;
    }

    function volumePercent(volume)
    {
        return Math.round(volume / PA.PulseAudio.NormalVolume * 100.0);
    }

    function disableGlobalMute()
    {
        var role = paSinkModel.role("Muted");
        for (var i = 0; i < paSinkModel.rowCount(); i++)
        {
            var idx = paSinkModel.index(i, 0);
            var name = paSinkModel.data(idx, paSinkModel.role("Name")) + "." + paSinkModel.data(idx, paSinkModel.role("ActivePortIndex"));
            paSinkModel.setData(idx, false, role);
        }
        globalMute = false;
    }

    function boundVolume(volume)
    {
            return Math.max(PA.PulseAudio.MinimalVolume, Math.min(volume, currentMaxVolumeValue));
        }


    function isDummyOutput(output)
    {
        return output && output.name === dummyOutputName;
    }
}
