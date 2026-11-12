import QtQuick
import QtQuick.Controls

import org.mauikit.controls as Maui

import org.maui.cask as Cask

Pane
{
    id: control
    implicitWidth: 300
    implicitHeight: _layout.implicitHeight + topPadding +bottomPadding
    spacing: Maui.Style.space.big
    background: Item{}

    Column
    {
        id: _layout
        width: parent.width
        spacing: control.spacing

        Repeater
        {
            model: CaskShell.notifications ? CaskShell.notifications.notificationsModel : null
            delegate: NotificationItem
            {
                width: parent.width
                autoDismiss: !model.persistent
                title: model.summary
                message: model.body
                //                iconSource: model.iconName
                imageSource : model.image
                timeout: model.timeout
                urgency: model.urgency
                actions: model.actions
                notificationId: model.notificationId
                onDismissed: CaskShell.notifications.notificationsModel.close(model.notificationId)
                onExpired: CaskShell.notifications.notificationsModel.expired(model.notificationId)
            }
        }
    }
}
