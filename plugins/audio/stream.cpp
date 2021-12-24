/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "stream.h"

namespace QPulseAudio
{
Stream::Stream(QObject *parent)
    : VolumeObject(parent)
    , m_deviceIndex(PA_INVALID_INDEX)
    , m_clientIndex(PA_INVALID_INDEX)
    , m_virtualStream(false)
    , m_corked(false)
{
    m_volumeWritable = false;
    m_hasVolume = false;
}

Stream::~Stream()
{
}

QString Stream::name() const
{
    return m_name;
}

Client *Stream::client() const
{
    return context()->clients().data().value(m_clientIndex, nullptr);
}

bool Stream::isVirtualStream() const
{
    return m_virtualStream;
}

quint32 Stream::deviceIndex() const
{
    return m_deviceIndex;
}

bool Stream::isCorked() const
{
    return m_corked;
}

} // QPulseAudio
