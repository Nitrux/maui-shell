import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3

import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.2 as Maui
import org.maui.cask 1.0 as Cask

Cask.PanelItem
{
    id: control
    icon.name: "notifications"
    text: _nof.count


//    Maui.Badge
//    {
//        height: parent.height
//        text: _nof.count
//        radius: 4
//    }

    card: Cask.PanelCard
    {
        width: ListView.view.width
        padding: 0
        //                    title: _nof.count + " " + qsTr("Notifications")
        //                    headBar.visible: true
        //                    headBar.leftContent: ToolButton
        //                    {
        //                        icon.name: "configure"
        //                    }

        //                    headBar.rightContent: ToolButton
        //                    {
        //                        icon.name: "edit-clear-all"
        //                    }

        ListView
        {
            id: _nof
            width: parent.width
            implicitHeight: Math.min(500, contentHeight)
            boundsBehavior: ListView.StopAtBounds
            model: 10
            spacing: Maui.Style.space.medium
            delegate: Maui.ItemDelegate
            {
                width: parent.width
                height: 80

                Maui.ListItemTemplate
                {
                    anchors.fill: parent
                    iconSource: "documentinfo"
                    label1.text: "Notification Title"
                    label2.text: "Blach some infor about the notification"
                    iconSizeHint: Maui.Style.iconSizes.medium
                    spacing: Maui.Style.space.medium
                    headerSizeHint: iconSizeHint + Maui.Style.space.big
                }

                onClicked: _nof.model--

            }
        }
    }
}
