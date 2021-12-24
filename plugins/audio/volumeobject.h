/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef VOLUMEOBJECT_H
#define VOLUMEOBJECT_H

#include <pulse/volume.h>

#include "pulseobject.h"

namespace QPulseAudio
{
class VolumeObject : public PulseObject
{
    Q_OBJECT
    Q_PROPERTY(qint64 volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(bool muted READ isMuted WRITE setMuted NOTIFY mutedChanged)
    Q_PROPERTY(bool hasVolume READ hasVolume NOTIFY hasVolumeChanged)
    Q_PROPERTY(bool volumeWritable READ isVolumeWritable NOTIFY isVolumeWritableChanged)
    Q_PROPERTY(QStringList channels READ channels NOTIFY channelsChanged)
    Q_PROPERTY(QStringList rawChannels READ rawChannels NOTIFY rawChannelsChanged)
    Q_PROPERTY(QVector<qint64> channelVolumes READ channelVolumes WRITE setChannelVolumes NOTIFY channelVolumesChanged)
public:
    explicit VolumeObject(QObject *parent);
    ~VolumeObject() override;

    template<typename PAInfo>
    void updateVolumeObject(PAInfo *info)
    {
        updatePulseObject(info);
        if (m_muted != info->mute) {
            m_muted = info->mute;
            Q_EMIT mutedChanged();
        }
        if (!pa_cvolume_equal(&m_volume, &info->volume)) {
            m_volume = info->volume;
            Q_EMIT volumeChanged();
            Q_EMIT channelVolumesChanged();
        }
        QStringList infoChannels;
        infoChannels.reserve(info->channel_map.channels);
        for (int i = 0; i < info->channel_map.channels; ++i) {
            infoChannels << QString::fromUtf8(pa_channel_position_to_pretty_string(info->channel_map.map[i]));
        }
        if (m_channels != infoChannels) {
            m_channels = infoChannels;
            Q_EMIT channelsChanged();
        }

        QStringList infoRawChannels;
        infoRawChannels.reserve(info->channel_map.channels);
        for (int i = 0; i < info->channel_map.channels; ++i) {
            infoRawChannels << QString::fromUtf8(pa_channel_position_to_string(info->channel_map.map[i]));
        }
        if (m_rawChannels != infoRawChannels) {
            m_rawChannels = infoRawChannels;
            Q_EMIT rawChannelsChanged();
        }
    }

    qint64 volume() const;
    virtual void setVolume(qint64 volume) = 0;

    bool isMuted() const;
    virtual void setMuted(bool muted) = 0;

    bool hasVolume() const;
    bool isVolumeWritable() const;

    QStringList channels() const;
    QStringList rawChannels() const;

    QVector<qint64> channelVolumes() const;
    virtual void setChannelVolumes(const QVector<qint64> &channelVolumes) = 0;
    Q_INVOKABLE virtual void setChannelVolume(int channel, qint64 volume) = 0;

Q_SIGNALS:
    void volumeChanged();
    void mutedChanged();
    void hasVolumeChanged();
    void isVolumeWritableChanged();
    void channelsChanged();
    void rawChannelsChanged();
    void channelVolumesChanged();

protected:
    pa_cvolume cvolume() const;

    pa_cvolume m_volume;
    bool m_muted;
    bool m_hasVolume;
    bool m_volumeWritable;
    QStringList m_channels;
    QStringList m_rawChannels;
};

} // QPulseAudio

#endif // VOLUMEOBJECT_H
