import QtQuick 2.15
import QtQuick.Controls 2.5

import org.mauikit.controls 1.2 as Maui

import org.maui.cask 1.0 as Cask
import "../../../../templates"

//import org.cask.audio 1.0 as CaskAudio

StackPage
{
    id: control
    implicitHeight: _listView.contentHeight + topPadding + bottomPadding + headerContainer.implicitHeight + footerContainer.implicitHeight + Maui.Style.space.big
    title: i18n("Microphone")

    Maui.ListBrowser
    {
        id: _listView
        anchors.fill: parent

        holder.visible: _listView.count === 0
        holder.title: i18n("Nothing here")
        holder.body: i18n("No output or input devices found.")
        holder.emoji: "code-context"

        model: paSourceFilterModel

        delegate: DeviceItem
        {
            width: ListView.view.width
            type: "source"
        }

    }
}
