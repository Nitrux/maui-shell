/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "client.h"

#include "debug.h"

namespace QPulseAudio
{
Client::Client(QObject *parent)
    : PulseObject(parent)
{
}

Client::~Client()
{
}

void Client::update(const pa_client_info *info)
{
    updatePulseObject(info);

    QString infoName = QString::fromUtf8(info->name);
    if (m_name != infoName) {
        m_name = infoName;
        Q_EMIT nameChanged();
    }
}

QString Client::name() const
{
    return m_name;
}

} // QPulseAudio
