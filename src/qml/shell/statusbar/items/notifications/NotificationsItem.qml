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
    text: _nofList.count


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
            id: _nofList
            width: parent.width
            implicitHeight: Math.max(300, Math.min(500, contentHeight))
            boundsBehavior: ListView.StopAtBounds
            model: _notifications.notificationsModel.historyModel
            spacing: Maui.Style.space.medium

            Maui.Holder
            {
                anchors.fill: parent
                visible: _nofList.count === 0
                emoji: "notifications"
                title: i18n("Clear")
                body: i18n("No notifications.")
            }

            delegate: NotificationItem
            {
                width: ListView.view.width
                autoDismiss: false
                title: model.title
                message: model.body
                iconSource: model.iconName
//                onDismissed:_nof.model--
//                onClicked: _nof.model--
            }
        }
    }
}
