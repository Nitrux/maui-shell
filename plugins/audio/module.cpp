/*
    SPDX-FileCopyrightText: 2017 David Rosca <nowrep@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "module.h"

#include "debug.h"

#include "context.h"

namespace QPulseAudio
{
Module::Module(QObject *parent)
    : PulseObject(parent)
{
}

void Module::update(const pa_module_info *info)
{
    updatePulseObject(info);

    const QString infoName = QString::fromUtf8(info->name);
    if (m_name != infoName) {
        m_name = infoName;
        Q_EMIT nameChanged();
    }
    const QString infoArgument = QString::fromUtf8(info->argument);
    if (m_argument != infoArgument) {
        m_argument = infoArgument;
        Q_EMIT argumentChanged();
    }
}

QString Module::name() const
{
    return m_name;
}

QString Module::argument() const
{
    return m_argument;
}

} // QPulseAudio
