import QtQuick
import QtQuick.Controls

import org.mauikit.controls as Maui

import org.maui.cask as Cask

import "../../../../../templates"

StackPage
{
    id: control

    implicitHeight: Math.max(400, Math.min(400, _listView.contentHeight + topPadding + bottomPadding + headerContainer.implicitHeight + footerContainer.implicitHeight + Maui.Style.space.big))
    title: i18n("Devices")

    headBar.rightContent: [
        Switch
        {
            checkable: true
        }
    ]

    Maui.ListBrowser
    {
        id: _listView
        anchors.fill: parent

        holder.visible: _listView.count === 0
        holder.title: i18n("Nothing here")
        holder.body: i18n("Under development. Check later.")
        holder.emoji: "code-context"
    }
}
