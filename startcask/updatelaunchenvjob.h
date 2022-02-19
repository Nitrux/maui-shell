/*
    SPDX-FileCopyrightText: 2020 Kai Uwe Broulik <kde@broulik.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <KJob>
#include <QProcessEnvironment>

class QString;

/**
 * Job for updating the launch environment.
 *
 * This job adds or updates an environment variable in process environment that will be used
 * anywhere a process is launched, e.g. DBus-activation environment, KLauncher environment, etc.
 *
 * @code
 * UpdateLaunchEnvJob("XCURSOR_THEME", "NewTheme");
 * @endcode
 *
 * @since 5.19
 */
class UpdateLaunchEnvJob : public KJob
{
    Q_OBJECT

public:
    explicit UpdateLaunchEnvJob(const QString &varName, const QString &value);
    explicit UpdateLaunchEnvJob(const QProcessEnvironment &environment);
    ~UpdateLaunchEnvJob() override;

    void start() override;

private:
    class Private;
    Private *const d;
};
