import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.12

import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.2 as Maui

import org.maui.cask 1.0 as Cask
import org.kde.bluezqt 1.0 as BluezQt
import org.kde.plasma.private.bluetooth 1.0 as PlasmaBt

import "../../../../../templates"

StackPage
{
    id: control

    implicitHeight: Math.max(200, Math.min(200, _layout.implicitHeight + topPadding + bottomPadding + headerContainer.implicitHeight + footerContainer.implicitHeight + Maui.Style.space.big))
    title: i18n("Screenshot")
    property int display: ToolButton.TextUnderIcon
    RowLayout
    {
        id: _layout
        width: control.width
        anchors.centerIn: parent
        spacing: Maui.Style.space.medium

        CardButton
        {
            text: i18n("Screen")
            icon.name: "view-fullscreen"
            display: control.display
            Layout.fillHeight: true
            Layout.fillWidth: true
            onClicked:
            {
                console.log("take screenshot")
                 grabScreen()
            }
        }

        CardButton
        {
            icon.name: "window"
            text: i18n("Window")
            display: control.display

            Layout.fillHeight: true
            Layout.fillWidth: true
        }

        CardButton
        {
            icon.name: "select-rectangular"
            text: i18n("Area")
            display: control.display

            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }
}
