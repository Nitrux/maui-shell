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

    card: Cask.PanelCard
    {
        width: ListView.view.width

        Row
        {
            ToolButton
            {
                icon.name: "system-log-out"
                onClicked: Qt.quit()
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
