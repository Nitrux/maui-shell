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
        message: i18n("Are you sure you want to quit the session and terminate runnig tasks?")
        page.margins: Maui.Style.space.big
        spacing: Maui.Style.space.medium
        acceptButton.text: _runningTasks.count > 0 ? i18n("Terminate") : i18n("Quit")

        ListView
        {
            id: _runningTasks
//            visible : count > 0

            Maui.Holder
            {
                anchors.fill: parent
                visible: _runningTasks.count === 0
                title: i18n("All Clear")
                body: i18n("Session can quitt")
            }

            Layout.fillWidth: true
            implicitHeight: Math.max(contentHeight + Maui.Style.space.big, 200)

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
             _sessioDialog.close()
        }
    }

    card: Cask.PanelCard
    {
        width: ListView.view.width

        Row
        {
            property int display: ToolButton.TextUnderIcon
            anchors.horizontalCenter: parent.horizontalCenter
            ToolButton
            {
                icon.name: "system-log-out"
                text: i18n("Quit")
                display: parent.display

                onClicked:
                {
                   _sessioDialog.open()
                }
            }

            ToolButton
            {
                icon.name: "system-reboot"
                text: i18n("Restart")
                display: parent.display

                onClicked:
                {
                }
            }

            ToolButton
            {
                icon.name: "system-shutdown"
                text: i18n("Shutdown")
                display: parent.display

                onClicked:
                {
                }
            }
        }
    }
}
