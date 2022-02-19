/*
    SPDX-FileCopyrightText: 2021 Henri Chain <henri.chain@enioka.com>
    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#pragma once

#include <KDesktopFile>
#include <QDir>
#include <QString>

/**
 * Corresponds to a .desktop file in $XDG_CONFIG_HOME/autostart that points to
 * an autostart script and has X-KDE-AutostartScript=true
 */
class AutostartScriptDesktopFile : public KDesktopFile
{
public:
    explicit AutostartScriptDesktopFile(const QString &name, const QString &execPath);

    /**
     * Checks whether this KDesktopFile has X-KDE-AutostartScript=true
     */
    static bool isAutostartScript(const KDesktopFile &file);

    /**
     * The location of autostart .desktop application and script files
     * ($XDG_CONFIG_HOME/autostart)
     */
    static QDir autostartLocation();
};
