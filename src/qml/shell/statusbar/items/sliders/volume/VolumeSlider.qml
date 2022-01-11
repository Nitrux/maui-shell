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

SliderToggle
{
    id: control
    property var defaultSinkValue: defaultSink ? defaultSink.volume / PA.PulseAudio.MaximalVolume * 100.0 : -1
    property var defaultSink: paSinkModel.defaultSink

    slider.iconSource: volumeIconName
    page: VolumePage
    {

    }

    slider.from: PA.PulseAudio.MinimalVolume
    slider.to: PA.PulseAudio.MaximalVolume
    slider.stepSize: slider.to / (slider.to / PA.PulseAudio.MaximalVolume * 100.0)

    slider.value: defaultSink ? defaultSink.volume : 0
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

        if (sinkIndex == undefined) {
            sinkIndex = paSinkModel.preferredSink.index;
        }
        feedback.play(sinkIndex);
    }

}
