/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "volumeosd.h"

#include "osdservice.h"

#define SERVICE QLatin1String("org.kde.plasmashell")
#define PATH QLatin1String("/org/kde/osdService")
#define CONNECTION QDBusConnection::sessionBus()

VolumeOSD::VolumeOSD(QObject *parent)
    : QObject(parent)
{
}

void VolumeOSD::show(int percent, int maximumPercent)
{
    OsdServiceInterface osdService(SERVICE, PATH, CONNECTION);
    osdService.volumeChanged(percent, maximumPercent);
}

void VolumeOSD::showMicrophone(int percent)
{
    OsdServiceInterface osdService(SERVICE, PATH, CONNECTION);
    osdService.microphoneVolumeChanged(percent);
}

void VolumeOSD::showText(const QString &iconName, const QString &text)
{
    OsdServiceInterface osdService(SERVICE, PATH, CONNECTION);
    osdService.showText(iconName, text);
}
