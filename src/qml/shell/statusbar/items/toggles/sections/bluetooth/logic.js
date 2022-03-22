//.pragma library

/*
    SPDX-FileCopyrightText: 2014-2015 David Rosca <nowrep@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

function updateStatus() {
    var connectedDevices = [];

    for (var i = 0; i < btManager.devices.length; ++i) {
        var device = btManager.devices[i];
        if (device.connected) {
            connectedDevices.push(device);
        }
    }

    var text = "";
    var bullet = "\u2022";

    if (btManager.bluetoothBlocked) {
        text = i18n("Bluetooth is disabled");
    } else if (!btManager.bluetoothOperational) {
        if (!btManager.adapters.length) {
            text = i18n("No adapters available");
        } else {
            text = i18n("Bluetooth is offline");
        }
    } else if (connectedDevices.length === 1) {
        text = i18n("%1 connected", connectedDevices[0].name);
    } else if (connectedDevices.length > 1) {
        text = i18ncp("Number of connected devices", "%1 connected device", "%1 connected devices", connectedDevices.length);
        for (var i = 0; i < connectedDevices.length; ++i) {
            var device = connectedDevices[i];
            text += "\n %1 %2".arg(bullet).arg(device.name);
        }
    } else {
        text = i18n("No connected devices");
    }

    tooltipText = text;
    deviceConnected = connectedDevices.length;

    if (btManager.bluetoothOperational)
    {
        status = 1;
    } else
    {
        status = 0;
    }
}

function icon()
{
    if (deviceConnected) {
        return "network-bluetooth-activated";
    } else if (!btManager.bluetoothOperational) {
        return "network-bluetooth";
    }
    return "network-bluetooth";
}

function conectedDevicesCount() {
  var connectedDevices = [];

  for (var i = 0; i < btManager.devices.length; ++i) {
    var device = btManager.devices[i];
    if (device.connected) {
      connectedDevices.push(device);
    }
  }

  return connectedDevices.length
}
