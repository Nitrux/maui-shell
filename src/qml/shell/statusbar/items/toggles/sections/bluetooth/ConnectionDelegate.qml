import QtQuick 2.15
import QtQuick.Controls 2.15

import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.2 as Maui

import org.maui.cask 1.0 as Cask

import org.kde.bluezqt 1.0 as BluezQt
import org.kde.plasma.private.bluetooth 1.0 as PlasmaBt

Maui.ListBrowserDelegate
{
    property bool connecting : false
    property var currentDeviceDetails : []

    iconSource: model.Icon
    checked: model.Connected

    label1.text: model.DeviceFullName
    label2.text: infoText()

    onToggled: connectToDevice()
    onClicked: connectToDevice()

    BusyIndicator
    {
        id: busyIndicator
        running: connecting
    }

    function infoText()
    {
        if (connecting) {
            return Connected ? i18n("Disconnecting") : i18n("Connecting");
        }

        var labels = [];

        if (Connected) {
            labels.push(i18n("Connected"));
        }

        switch (Type) {
        case BluezQt.Device.Headset:
        case BluezQt.Device.Headphones:
        case BluezQt.Device.OtherAudio:
            labels.push(i18n("Audio device"));
            break;

        case BluezQt.Device.Keyboard:
        case BluezQt.Device.Mouse:
        case BluezQt.Device.Joypad:
        case BluezQt.Device.Tablet:
            labels.push(i18n("Input device"));
            break;

        case BluezQt.Device.Phone:
            labels.push(i18n("Phone"));
            break;

        default:
            var profiles = [];

            if (Uuids.indexOf(BluezQt.Services.ObexFileTransfer) !== -1) {
                profiles.push(i18n("File transfer"));
            }
            if (Uuids.indexOf(BluezQt.Services.ObexObjectPush) !== -1) {
                profiles.push(i18n("Send file"));
            }
            if (Uuids.indexOf(BluezQt.Services.HumanInterfaceDevice) !== -1) {
                profiles.push(i18n("Input"));
            }
            if (Uuids.indexOf(BluezQt.Services.AdvancedAudioDistribution) !== -1) {
                profiles.push(i18n("Audio"));
            }
            if (Uuids.indexOf(BluezQt.Services.Nap) !== -1) {
                profiles.push(i18n("Network"));
            }

            if (!profiles.length) {
                profiles.push(i18n("Other device"));
            }

            labels.push(profiles.join(", "));
        }

        if (Battery) {
            labels.push(i18n("%1% Battery", Battery.percentage));
        }

        return labels.join(" · ");
    }

    function boolToString(v)
       {
           if (v) {
               return i18n("Yes");
           }
           return i18n("No");
       }

       function adapterName(a)
       {
           var hci = devicesModel.adapterHciString(a.ubi);
           if (hci !== "") {
               return "%1 (%2)".arg(a.name).arg(hci);
           }
           return a.name;
       }

       function createContent() {
           var details = [];

           if (Name !== RemoteName) {
               details.push(i18n("Remote Name"));
               details.push(RemoteName);
           }

           details.push(i18n("Address"));
           details.push(Address);

           details.push(i18n("Paired"));
           details.push(boolToString(Paired));

           details.push(i18n("Trusted"));
           details.push(boolToString(Trusted));

           details.push(i18n("Adapter"));
           details.push(adapterName(Adapter));

           currentDeviceDetails = details;
       }


       function connectToDevice()
       {
           if (connecting) {
               return;
           }

           connecting = true;
           runningActions++;

           // Disconnect device
           if (Connected) {
               Device.disconnectFromDevice().finished.connect(function(call) {
                   connecting = false;
                   runningActions--;
               });
               return;
           }

           // Connect device
           var call = Device.connectToDevice();
           call.userData = Device;

           call.finished.connect(function(call) {
               connecting = false;
               runningActions--;

               if (call.error) {
                   var text = "";
                   var device = call.userData;
                   var title = "%1 (%2)".arg(device.name).arg(device.address);

                   switch (call.error) {
                   case BluezQt.PendingCall.Failed:
                       if (call.errorText === "Host is down") {
                           text = i18nc("Notification when the connection failed due to Failed:HostIsDown",
                                        "The device is unreachable");
                       } else {
                           text = i18nc("Notification when the connection failed due to Failed",
                                        "Connection to the device failed");
                       }
                       break;

                   case BluezQt.PendingCall.NotReady:
                       text = i18nc("Notification when the connection failed due to NotReady",
                                    "The device is not ready");
                       break;

                   default:
                       return;
                   }

                   PlasmaBt.Notify.connectionFailed(title, text);
               }
           });
       }


}
