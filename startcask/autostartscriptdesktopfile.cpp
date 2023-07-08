/*
    SPDX-FileCopyrightText: 2021 Henri Chain <henri.chain@enioka.com>
    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "autostartscriptdesktopfile.h"
#include <KConfigGroup>
#include <KDesktopFile>
#include <QDir>
#include <QStandardPaths>

static const auto autostartScriptKey = QStringLiteral("X-KDE-AutostartScript");

QDir AutostartScriptDesktopFile::autostartLocation()
{
    return QDir(QDir(QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation)).filePath(QStringLiteral("autostart")));
}

AutostartScriptDesktopFile::AutostartScriptDesktopFile(const QString &name, const QString &execPath)
    : KDesktopFile(autostartLocation().absoluteFilePath(name + QStringLiteral(".desktop")))
{
    KConfigGroup kcg = desktopGroup();
    kcg.writeEntry(QStringLiteral("Type"), "Application");
    kcg.writeEntry(QStringLiteral("Name"), name);
    kcg.writeEntry(QStringLiteral("Exec"), execPath);
    kcg.writeEntry(QStringLiteral("Icon"), "dialog-scripts");
    kcg.writeEntry(autostartScriptKey, "true");
    kcg.writeEntry(QStringLiteral("Path"), "");
}

bool AutostartScriptDesktopFile::isAutostartScript(const KDesktopFile &file)
{
    return file.desktopGroup().readEntry<bool>(autostartScriptKey, false);
}
