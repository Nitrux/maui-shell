import QtQuick 2.15
import QtQuick.Controls 2.15

import org.kde.kirigami 2.7 as Kirigami
import org.mauikit.controls 1.2 as Maui

import org.maui.cask 1.0 as Cask
import org.cask.notifications 1.0 as Nof
Pane
{
    id: control
    implicitWidth: 300
    implicitHeight: _layout.implicitHeight + topPadding +bottomPadding
    spacing: Maui.Style.space.big
    background: Item{}

    Nof.NotificationsServer {
           onActiveChanged: {
               if (active)
                   console.debug("Notifications manager activated");
               else
                   console.debug("Notifications manager deactivated");
           }

           onNotificationReceived: {
               console.debug("Notification", notificationId, "received");
               var props = {
                   "notificationId": notificationId,
                   "appName": appName,
                   "appIcon": appIcon,
                   "iconUrl": "image://notifications/%1/%2".arg(notificationId).arg(Date.now() / 1000 | 0),
                   "hasIcon": hasIcon,
                   "summary": summary,
                   "body": body,
                   "isPersistent": isPersistent,
                   "expireTimeout": expireTimeout,
                   "hints": hints,
                   "actions": actions,
               };
               _notificationsModel.append(props);
           }

           onNotificationClosed: {
               console.debug("Notification", notificationId, "closed for", reason);
           }

           onActionInvoked: {
               console.debug("Notification", notificationId, "action:", actionKey);
           }
       }
    Column
    {
        id: _layout
        width: parent.width
        spacing: control.spacing

        Repeater
        {
            model:_notificationsModel
            delegate: NotificationItem
            {
                width: parent.width
                autoDismiss: true
                onDismissed: _notificationsModel.remove(index)
            }
        }
    }

}
