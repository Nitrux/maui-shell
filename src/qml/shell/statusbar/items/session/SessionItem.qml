import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3

import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.2 as Maui
import org.maui.cask 1.0 as Cask
import org.kde.plasma.core 2.0 as PlasmaCore

Cask.PanelItem
{
    icon.name: "battery-080"
    text: "80%"
    display: isMobile ? ToolButton.IconOnly : ToolButton.TextBesideIcon

    Maui.Dialog
    {
        id: _sessioDialog
        title: i18n("Quit Session")
        message: i18n("Are you sure you want to quit the session and terminate runnig  tasks?")
        page.margins: Maui.Style.space.big
        spacing: Maui.Style.space.medium
        ListView
        {
            Layout.fillWidth: true
            implicitHeight: 200
            model: _zpaces.allSurfaces
            delegate: Maui.ListBrowserDelegate
            {
                isCurrentItem: false
                background: null
                width: ListView.view.width
                label1.text: model.window.title
                label2.text: model.window.appName
                iconSource: model.window.iconName
            }
        }

        onAccepted:
        {
            _zpaces.clearAllSurfaces()
            if(_zpaces.allSurfaces.count === 0)
            {
                Qt.quit()
            }

        }

        onRejected:
        {
             Qt.quit()
        }
    }

    card: Cask.PanelCard
    {
        width: ListView.view.width

        Row
        {
            ToolButton
            {
                icon.name: "system-log-out"
                onClicked:
                {
                   _sessioDialog.open()
                }
            }

            Repeater
            {
                model: ["system-reboot", "system-shutdown", "system-lock-screen","webcam", "system-suspend"]
                delegate:  ToolButton
                {

                    icon.name: modelData
                }
            }
        }
    }
}
