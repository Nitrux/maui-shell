import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5

import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.2 as Maui

import org.maui.cask 1.0 as Cask
import "../.."

TogglePage
{
    id: control
    implicitHeight: _listView.contentHeight + topPadding + bottomPadding + headerContainer.implicitHeight + footerContainer.implicitHeight + Maui.Style.space.big
    headerColorSet: control.Kirigami.Theme.colorSet
    title: "Networks"
    showTitle: true
    headBar.rightContent: Switch
    {
        checkable: true
        checked: true
    }

    Maui.ListBrowser
    {
        id: _listView
        anchors.fill: parent
        model: ["camilog", "Camilo-5G", "xiaomi", "Familia Rodriguez", "peng-h"]

        delegate: Maui.ListBrowserDelegate
        {
            width: ListView.view.width
            checked: ListView.isCurrentItem

            label1.text: modelData
            label1.font.bold: true
            label1.font.pointSize: Maui.Style.fontSizes.big
            label2.text: "Network information"
            iconSource: "network-wireless"
            iconSizeHint: 16

            Kirigami.Icon
            {
                implicitHeight: 16
                implicitWidth: implicitHeight
                source: "object-locked"
                color: Kirigami.Theme.textColor
            }
        }
    }
}
