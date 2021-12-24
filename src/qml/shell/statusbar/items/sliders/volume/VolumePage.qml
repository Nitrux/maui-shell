import QtQuick 2.15
import QtQuick.Controls 2.5

import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.2 as Maui

import org.maui.cask 1.0 as Cask
import ".."

SliderPage
{
    id: control
    implicitHeight: Math.max(400, Math.min(400, _listView.contentHeight + topPadding + bottomPadding + headerContainer.implicitHeight + footerContainer.implicitHeight + Maui.Style.space.big))
showTitle: true
title: i18n("Audio")

    Maui.ListBrowser
    {
        id: _listView
        anchors.fill: parent
    }
}
