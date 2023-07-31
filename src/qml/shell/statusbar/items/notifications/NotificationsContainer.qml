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
            model: _notifications.notificationsModel
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
                onDismissed: _notifications.notificationsModel.close(model.notificationId)
                onExpired: _notifications.notificationsModel.expired(model.notificationId)
            }
        }
    }
}
