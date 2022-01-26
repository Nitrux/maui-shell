import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import QtQuick.Templates 2.15 as T

import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.2 as Maui
import org.maui.cask 1.0 as Cask
import org.kde.plasma.core 2.0 as PlasmaCore
import "../../../templates"

Cask.PanelItem
{   
    id: control
    display: isMobile ? ToolButton.IconOnly : ToolButton.TextBesideIcon

    Row
    {
        spacing: control.spacing
        Kirigami.Icon
        {
            source: "battery-080"
            height: control.iconSize
            width: height
            color: control.icon.color
        }

        Kirigami.Icon
        {
            source: "view-media-artist"
            height: control.iconSize
            width: height
            color: control.icon.color
        }
    }

    CaskDialog
    {
        id: _sessioDialog
        title: i18n("Quit Session")
        message: i18n("Are you sure you want to quit the session and terminate runnig tasks?")
        template.iconSource: "emblem-warning"
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
            Layout.fillHeight: true
            Layout.minimumHeight: Math.max(contentHeight + Maui.Style.space.big, 200)

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
        onClosed: _stack.pop()

        T.StackView
        {
            id: _stack

            width: parent.width
            height: currentItem.implicitHeight
            clip: true
            padding: 0
            leftPadding: padding
            rightPadding: padding
            topPadding: padding
            bottomPadding: padding
            initialItem: Column
            {
                spacing: control.card.spacing

                BatteryToggle
                {
                    id: _batteryProgressBar
                    width: parent.width
                    onClicked: _stack.push(page)
                }

                RowLayout
                {
                    width: parent.width
                    property int display: ToolButton.TextUnderIcon
                    spacing: Maui.Style.space.medium

                    SessionButton
                    {
                        Layout.fillWidth: true
                        icon.name: "system-log-out"
                        text: i18n("Quit")
                        display: parent.display

                        onClicked:
                        {
                            _sessioDialog.open()
                        }
                    }

                    SessionButton
                    {
                        Layout.fillWidth: true

                        icon.name: "system-reboot"
                        text: i18n("Restart")
                        display: parent.display

                        onClicked:
                        {
                        }
                    }

                    SessionButton
                    {
                        Layout.fillWidth: true

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
    }
}
