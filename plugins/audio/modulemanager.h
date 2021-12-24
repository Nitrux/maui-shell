/*
    SPDX-FileCopyrightText: 2016 David Edmundson <davidedmundson@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef MODULEMANAGER_H
#define MODULEMANAGER_H

#include <QString>

#include <pulse/introspect.h>

#include "context.h"
// Properties need fully qualified classes even with pointers.
#include "client.h"

namespace QPulseAudio
{
class ConfigModule;

class ModuleManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool settingsSupported READ settingsSupported NOTIFY serverUpdated)
    Q_PROPERTY(bool combineSinks READ combineSinks WRITE setCombineSinks NOTIFY combineSinksChanged)
    Q_PROPERTY(bool switchOnConnect READ switchOnConnect WRITE setSwitchOnConnect NOTIFY switchOnConnectChanged)
    Q_PROPERTY(bool configModuleLoaded READ configModuleLoaded NOTIFY loadedModulesChanged)
    Q_PROPERTY(QString configModuleName READ configModuleName CONSTANT)
    Q_PROPERTY(QStringList loadedModules READ loadedModules NOTIFY loadedModulesChanged)
public:
    explicit ModuleManager(QObject *parent = nullptr);
    ~ModuleManager() override;

    bool settingsSupported() const;
    bool combineSinks() const;
    void setCombineSinks(bool combineSinks);
    bool switchOnConnect() const;
    void setSwitchOnConnect(bool switchOnConnect);
    QStringList loadedModules() const;
    bool configModuleLoaded() const;
    QString configModuleName() const;

Q_SIGNALS:
    void combineSinksChanged();
    void switchOnConnectChanged();
    void loadedModulesChanged();
    void serverUpdated();

private:
    void updateLoadedModules();

    ConfigModule *m_combineSinks;
    ConfigModule *m_switchOnConnect;
    ConfigModule *m_deviceManager;
    QStringList m_loadedModules;
};

} // QPulseAudio

#endif // STREAM_H
