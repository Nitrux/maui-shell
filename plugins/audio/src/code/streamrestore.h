/*
    SPDX-FileCopyrightText: 2016 David Rosca <nowrep@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef STREAMRESTORE_H
#define STREAMRESTORE_H

#include "pulseobject.h"

#include <pulse/ext-stream-restore.h>

namespace QPulseAudio
{
class StreamRestore : public PulseObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString device READ device WRITE setDevice NOTIFY deviceChanged)
    Q_PROPERTY(qint64 volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(bool muted READ isMuted WRITE setMuted NOTIFY mutedChanged)
    Q_PROPERTY(bool hasVolume READ hasVolume CONSTANT)
    Q_PROPERTY(bool volumeWritable READ isVolumeWritable CONSTANT)
    Q_PROPERTY(QStringList channels READ channels NOTIFY channelsChanged)
    Q_PROPERTY(QList<qreal> channelVolumes READ channelVolumes NOTIFY channelVolumesChanged)
    Q_PROPERTY(quint32 deviceIndex READ deviceIndex WRITE setDeviceIndex NOTIFY deviceIndexChanged)
public:
    StreamRestore(quint32 index, const QVariantMap &properties, QObject *parent);

    void update(const pa_ext_stream_restore_info *info);

    QString name() const;

    QString device() const;
    void setDevice(const QString &device);

    qint64 volume() const;
    void setVolume(qint64 volume);

    bool isMuted() const;
    void setMuted(bool muted);

    bool hasVolume() const;
    bool isVolumeWritable() const;

    QStringList channels() const;

    QList<qreal> channelVolumes() const;

    quint32 deviceIndex() const;
    void setDeviceIndex(quint32 deviceIndex);

    Q_INVOKABLE void setChannelVolume(int channel, qint64 volume);

Q_SIGNALS:
    void nameChanged();
    void deviceChanged();
    void volumeChanged();
    void mutedChanged();
    void channelsChanged();
    void channelVolumesChanged();
    void deviceIndexChanged();

private:
    void writeChanges(const pa_cvolume &volume, bool muted, const QString &device);

    QString m_name;
    QString m_device;
    pa_cvolume m_volume;
    pa_channel_map m_channelMap;
    QStringList m_channels;
    bool m_muted;

    struct {
        bool valid = false;
        pa_cvolume volume;
        bool muted;
        QString device;
    } m_cache;
};

} // QPulseAudio

#endif // STREAMRESTORE_H
