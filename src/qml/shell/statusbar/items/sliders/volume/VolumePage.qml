import QtQuick 2.15
import QtQuick.Controls 2.5

import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.2 as Maui

import org.maui.cask 1.0 as Cask
import "../../../../templates"

StackPage
{
    id: control
    implicitHeight: Math.max(400, Math.min(400, _listView.contentHeight + topPadding + bottomPadding + headerContainer.implicitHeight + footerContainer.implicitHeight + Maui.Style.space.big))
    title: i18n("Audio")

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
