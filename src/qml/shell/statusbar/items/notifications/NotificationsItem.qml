import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.mauikit.controls as Maui
import org.maui.cask as Cask

Cask.PanelItem
{
    id: control

    icon.name: "notifications"
    text: _nofList.count > 0 ? _nofList.count : ""

    onTextChanged: animate()

    //    Maui.Badge
    //    {
    //        height: parent.height
    //        text: _nof.count
    //        radius: 4
    //    }

    card: Cask.PanelCard
    {
        width: ListView.view.width

        ListView
        {
            id: _nofList
            width: parent.width
            clip: true
            implicitHeight: Math.max(300, Math.min(500, contentHeight))
            boundsBehavior: ListView.StopAtBounds
            model: _notifications.notificationsModel.historyModel
            spacing: Maui.Style.space.medium

            section.delegate: Maui.LabelDelegate
            {
                width: parent.width
                label: section
                isSection: true
            }

            section.property: "category"
            section.criteria: ViewSection.FullString

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
                title: model.summary
                message: model.body
                iconSource: model.iconName
                imageSource : model.image
                onDismissed:_notifications.notificationsModel.historyModel.remove(index)
                background: Rectangle
                {
                    color: hovered? Qt.lighter(Maui.Theme.backgroundColor) : Maui.Theme.backgroundColor
                    radius: 10
                }
            }
        }

        Maui.ToolBar
        {
            width: parent.width
            background: null
            rightContent: [ToolButton
                {
                    visible:  _nofList.count !== 0
                    icon.name: "edit-clear-all"
                    onClicked: _notifications.notificationsModel.historyModel.clearAll()
                },

                ToolButton
                {
                    icon.name: "settings-configure"
                }
            ]

            leftContent: Switch
            {
//                text: "Silent"
                icon.name: checked? "notifications-disabled" : "notifications-enabled"
                checkable: true
                checked: _notifications.notificationsModel.doNotDisturb
                onToggled: _notifications.notificationsModel.doNotDisturb = checked

            }
        }
    }
}
