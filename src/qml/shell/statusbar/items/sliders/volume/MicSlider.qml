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
    property var defaultSourceValue: defaultSource ? defaultSource.volume / PA.PulseAudio.MaximalVolume * 100.0 : -1
    property var defaultSource: paSourceModel.defaultSource
    readonly property bool muted : defaultSource ?  defaultSource.muted : true

    enabled: !muted

    slider.iconSource: {
        if (defaultSourceValue <= 0)
            return "microphone-sensitivity-muted"
        else if (defaultSourceValue <= 25)
            return "microphone-sensitivity-low"
        else if (defaultSourceValue <= 75)
            return "microphone-sensitivity-medium"
        else
            return "microphone-sensitivity-high"
    }

    page: MicPage
    {

    }

    slider.from: PA.PulseAudio.MinimalVolume
    slider.to: PA.PulseAudio.MaximalVolume
    slider.stepSize: slider.to / (slider.to / PA.PulseAudio.MaximalVolume * 100.0)

    slider.value: defaultSource ? defaultSource.volume : 0
    Connections
    {
        target: control.slider
        function onValueChanged()
        {
            if (!defaultSource)
                return

            defaultSource.volume = target.value
            defaultSource.muted = (target.value === 0)
        }
    }

    // Input devices
    PA.SourceModel
    {
        id: paSourceModel
    }

    // active input devices
    PA.PulseObjectFilterModel
    {
        id: paSourceFilterModel
        sortRole: "SortByDefault"
        sortOrder: Qt.DescendingOrder
        filterOutInactiveDevices: true
        sourceModel: paSourceModel
    }

    PA.CardModel
    {
        id: paCardModel
    }

    function muteMic()
    {
        for (var i = 0; i < paSourceModel.rowCount(); i++) {

            paSourceModel.setData(paSourceModel.index(i, 0), true, paSourceModel.role("Muted"));

        }
    }

    function unmuteMic()
    {
        for (var i = 0; i < paSourceModel.rowCount(); i++) {

            paSourceModel.setData(paSourceModel.index(i, 0), false, paSourceModel.role("Muted"));

        }
    }

    function toggleMute()
    {
        if (control.muted)
        {
            control.unmuteMic()
        }else
        {
            control.muteMic()
        }
    }
}
