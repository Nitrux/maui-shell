/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "volumeobject.h"

namespace QPulseAudio
{
VolumeObject::VolumeObject(QObject *parent)
    : PulseObject(parent)
    , m_muted(true)
    , m_hasVolume(true)
    , m_volumeWritable(true)
{
    pa_cvolume_init(&m_volume);
}

VolumeObject::~VolumeObject()
{
}

qint64 VolumeObject::volume() const
{
    return pa_cvolume_max(&m_volume);
}

bool VolumeObject::isMuted() const
{
    return m_muted;
}

pa_cvolume VolumeObject::cvolume() const
{
    return m_volume;
}

bool VolumeObject::hasVolume() const
{
    return m_hasVolume;
}

bool VolumeObject::isVolumeWritable() const
{
    return m_volumeWritable;
}

QStringList VolumeObject::channels() const
{
    return m_channels;
}

QStringList VolumeObject::rawChannels() const
{
    return m_rawChannels;
}

QVector<qint64> VolumeObject::channelVolumes() const
{
    QVector<qint64> ret;
    ret.reserve(m_volume.channels);
    for (int i = 0; i < m_volume.channels; ++i) {
        ret << m_volume.values[i];
    }
    return ret;
}

} // QPulseAudio
