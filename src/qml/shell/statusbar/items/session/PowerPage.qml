import QtQuick 2.15
import QtQuick.Controls 2.5

import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.2 as Maui

import org.maui.cask 1.0 as Cask
import org.cask.power 1.0 as CaskPower
import "../../../templates"
import org.kde.plasma.private.volume 0.1 as PA

StackPage
{
    id: control
    implicitHeight: Math.max(400, Math.min(400, _layout.implicitHeight + topPadding + bottomPadding + headerContainer.implicitHeight + footerContainer.implicitHeight + Maui.Style.space.big))
    title: i18n("Brightness")
    property alias model : _listView.model

    Maui.Holder
    {
        anchors.fill: parent
        visible: _listView.count === 0
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
            delegate: BatteryBar
            {
                battery: model.Battery
                width: ListView.view.width
                label.text: battery.state
            }
        }
    }
}
