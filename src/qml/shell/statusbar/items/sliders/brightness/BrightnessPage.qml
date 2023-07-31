import QtQuick
import QtQuick.Controls

import org.mauikit.controls as Maui

import org.maui.cask as Cask
import "../../../../templates"

//import org.cask.audio 1.0 as CaskAudio

StackPage
{
    id: control
    implicitHeight: Math.max(400, Math.min(400, _layout.implicitHeight + topPadding + bottomPadding + headerContainer.implicitHeight + footerContainer.implicitHeight + Maui.Style.space.big))
    title: i18n("Brightness")

    Maui.Holder
    {
        anchors.fill: parent
        visible: _listView.count === 0 && _appsList.count === 0
        title: i18n("Nothing here")
        body: i18n("No devices found.")
        emoji: "code-context"
    }

    Column
    {
        id: _layout
        spacing: control.spacing
        anchors.fill: parent

        Maui.ListBrowser
        {
            id: _listView
            width: parent.width

//            model: 2

            delegate: ItemDelegate {}
        }
    }
}
