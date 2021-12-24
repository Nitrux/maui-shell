/*
    SPDX-FileCopyrightText: 2016 David Rosca <nowrep@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "streamrestore.h"
#include "context.h"
#include "debug.h"

namespace QPulseAudio
{
StreamRestore::StreamRestore(quint32 index, const QVariantMap &properties, QObject *parent)
    : PulseObject(parent)
    , m_muted(false)
{
    memset(&m_volume, 0, sizeof(m_volume));
    memset(&m_channelMap, 0, sizeof(m_channelMap));

    m_index = index;
    m_properties = properties;
}

void StreamRestore::update(const pa_ext_stream_restore_info *info)
{
    m_cache.valid = false;
    const QString infoName = QString::fromUtf8(info->name);
    if (m_name != infoName) {
        m_name = infoName;
        Q_EMIT nameChanged();
    }
    const QString infoDevice = QString::fromUtf8(info->device);
    if (m_device != infoDevice) {
        m_device = infoDevice;
        Q_EMIT deviceChanged();
    }
    if (m_muted != info->mute) {
        m_muted = info->mute;
        Q_EMIT mutedChanged();
    }
    if (!pa_cvolume_equal(&m_volume, &info->volume)) {
        m_volume = info->volume;
        Q_EMIT volumeChanged();
        Q_EMIT channelVolumesChanged();
    }
    if (!pa_channel_map_equal(&m_channelMap, &info->channel_map)) {
        m_channels.clear();
        m_channels.reserve(info->channel_map.channels);
        for (int i = 0; i < info->channel_map.channels; ++i) {
            m_channels << QString::fromUtf8(pa_channel_position_to_pretty_string(info->channel_map.map[i]));
        }
        m_channelMap = info->channel_map;
        Q_EMIT channelsChanged();
    }
}

QString StreamRestore::name() const
{
    return m_name;
}

QString StreamRestore::device() const
{
    return m_device;
}

void StreamRestore::setDevice(const QString &device)
{
    if (m_cache.valid) {
        if (m_cache.device != device) {
            writeChanges(m_cache.volume, m_cache.muted, device);
        }
    } else {
        if (m_device != device) {
            writeChanges(m_volume, m_muted, device);
        }
    }
}

qint64 StreamRestore::volume() const
{
    return m_volume.values[0];
}

void StreamRestore::setVolume(qint64 volume)
{
    pa_cvolume vol = m_cache.valid ? m_cache.volume : m_volume;

    // If no channel exists force one. We need one to be able to control the volume
    // See https://bugs.kde.org/show_bug.cgi?id=407397
    if (vol.channels == 0) {
        vol.channels = 1;
    }

    for (int i = 0; i < vol.channels; ++i) {
        vol.values[i] = volume;
    }

    if (m_cache.valid) {
        writeChanges(vol, m_cache.muted, m_cache.device);
    } else {
        writeChanges(vol, m_muted, m_device);
    }
}

bool StreamRestore::isMuted() const
{
    return m_muted;
}

void StreamRestore::setMuted(bool muted)
{
    if (m_cache.valid) {
        if (m_cache.muted != muted) {
            writeChanges(m_cache.volume, muted, m_cache.device);
        }
    } else {
        if (m_muted != muted) {
            writeChanges(m_volume, muted, m_device);
        }
    }
}

bool StreamRestore::hasVolume() const
{
    return true;
}

bool StreamRestore::isVolumeWritable() const
{
    return true;
}

QStringList StreamRestore::channels() const
{
    return m_channels;
}

QList<qreal> StreamRestore::channelVolumes() const
{
    QList<qreal> ret;
    ret.reserve(m_volume.channels);
    for (int i = 0; i < m_volume.channels; ++i) {
        ret << m_volume.values[i];
    }
    return ret;
}

void StreamRestore::setChannelVolume(int channel, qint64 volume)
{
    Q_ASSERT(channel >= 0 && channel < m_volume.channels);
    pa_cvolume vol = m_cache.valid ? m_cache.volume : m_volume;
    vol.values[channel] = volume;

    if (m_cache.valid) {
        writeChanges(vol, m_cache.muted, m_cache.device);
    } else {
        writeChanges(vol, m_muted, m_device);
    }
}

quint32 StreamRestore::deviceIndex() const
{
    return PA_INVALID_INDEX;
}

void StreamRestore::setDeviceIndex(quint32 deviceIndex)
{
    Q_UNUSED(deviceIndex);
    qCWarning(PLASMAPA) << "Not implemented";
}

void StreamRestore::writeChanges(const pa_cvolume &volume, bool muted, const QString &device)
{
    const QByteArray nameData = m_name.toUtf8();
    const QByteArray deviceData = device.toUtf8();

    pa_ext_stream_restore_info info;
    info.name = nameData.constData();
    info.channel_map = m_channelMap;
    info.volume = volume;
    info.device = deviceData.isEmpty() ? nullptr : deviceData.constData();
    info.mute = muted;

    // If no channel exists force one. We need one to be able to control the volume
    // See https://bugs.kde.org/show_bug.cgi?id=407397
    if (info.channel_map.channels == 0) {
        info.channel_map.channels = 1;
        info.channel_map.map[0] = PA_CHANNEL_POSITION_MONO;
    }

    m_cache.valid = true;
    m_cache.volume = volume;
    m_cache.muted = muted;
    m_cache.device = device;

    context()->streamRestoreWrite(&info);
}

} // QPulseAudio
