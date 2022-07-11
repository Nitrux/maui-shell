import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import QtQuick.Templates 2.15 as T

import org.mauikit.controls 1.3 as Maui
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
        Maui.Icon
        {
            visible: _batteryProgressBar.primaryBattery
            source: _batteryProgressBar.iconSource
            height: visible ? control.iconSize : 0
            width: height
            color: control.icon.color
        }

        Maui.Icon
        {
            source: "system-shutdown"
            height: control.iconSize
            width: height
            color: control.icon.color
        }
    }

    CaskDialog
    {
        id: _sessioDialog
        property string operation
        title: switch(operation)
               {
               case "logout": return i18n("Quit Session");
               case "reboot": return i18n("Restart");
               case "shutdown": return i18n("Shutdown");
               }

        message: i18np("Are you sure you want to quit the session?", "Are you sure you want to quit the session and terminate runnig tasks?", _runningTasks.count+1);


        template.iconSource: switch(operation)
                             {
                             case "logout": return i18n("system-log-out");
                             case "reboot": return i18n("system-reboot");
                             case "shutdown": return i18n("system-shutdown");
                             }

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
                body: i18n("Session can be exited")
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
                switch(_sessioDialog.operation)
                {
                case "logout": Cask.Server.power.logout(); break;
                case "reboot": Cask.Server.power.reboot(); break;
                case "shutdown": Cask.Server.power.shutdown(); break;
                }
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
                    property int display: ToolButton.OnlyIcon
                    spacing: Maui.Style.space.medium

                    SessionButton
                    {
                        Layout.fillWidth: true
                        icon.name: "system-log-out"
                        text: i18n("Quit")
                        display: parent.display

                        onClicked:
                        {
                            _sessioDialog.operation = "logout"
                            _sessioDialog.open()
                        }
                    }

                    SessionButton
                    {
                        Layout.fillWidth: true
                        icon.name: "system-switch-user-symbolic"
                        text: i18n("Switch")
                        display: parent.display

                        onClicked:
                        {

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
                            _sessioDialog.operation = "reboot"
                            _sessioDialog.open()
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
                            _sessioDialog.operation = "shutdown"
                            _sessioDialog.open()
                        }
                    }
                }
            }
        }
    }
}
