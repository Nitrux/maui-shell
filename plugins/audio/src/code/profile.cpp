/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "profile.h"

namespace QPulseAudio
{
Profile::Profile(QObject *parent)
    : QObject(parent)
    , m_name()
    , m_description()
    , m_priority(0)
    , m_availability(Unknown)
{
}

Profile::~Profile()
{
}

QString Profile::name() const
{
    return m_name;
}

QString Profile::description() const
{
    return m_description;
}

quint32 Profile::priority() const
{
    return m_priority;
}

Profile::Availability Profile::availability() const
{
    return m_availability;
}

} // QPulseAudio
