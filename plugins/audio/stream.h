/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef STREAM_H
#define STREAM_H

#include <QString>

#include <pulse/volume.h>

#include "pulseobject.h"
#include "volumeobject.h"

#include "context.h"
// Properties need fully qualified classes even with pointers.
#include "client.h"

namespace QPulseAudio
{
class Stream : public VolumeObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QPulseAudio::Client *client READ client NOTIFY clientChanged)
    Q_PROPERTY(bool virtualStream READ isVirtualStream NOTIFY virtualStreamChanged)
    Q_PROPERTY(quint32 deviceIndex READ deviceIndex WRITE setDeviceIndex NOTIFY deviceIndexChanged)
    Q_PROPERTY(bool corked READ isCorked NOTIFY corkedChanged)
public:
    template<typename PAInfo>
    void updateStream(const PAInfo *info)
    {
        updateVolumeObject(info);

        if (m_name != QString::fromUtf8(info->name)) {
            m_name = QString::fromUtf8(info->name);
            Q_EMIT nameChanged();
        }
        if (m_hasVolume != info->has_volume) {
            m_hasVolume = info->has_volume;
            Q_EMIT hasVolumeChanged();
        }
        if (m_volumeWritable != info->volume_writable) {
            m_volumeWritable = info->volume_writable;
            Q_EMIT isVolumeWritableChanged();
        }
        if (m_clientIndex != info->client) {
            m_clientIndex = info->client;
            Q_EMIT clientChanged();
        }
        if (m_virtualStream != (info->client == PA_INVALID_INDEX)) {
            m_virtualStream = info->client == PA_INVALID_INDEX;
            Q_EMIT virtualStreamChanged();
        }
        if (m_corked != info->corked) {
            m_corked = info->corked;
            Q_EMIT corkedChanged();
        }
    }

    QString name() const;
    Client *client() const;
    bool isVirtualStream() const;
    quint32 deviceIndex() const;
    bool isCorked() const;

    virtual void setDeviceIndex(quint32 deviceIndex) = 0;

Q_SIGNALS:
    void nameChanged();
    void clientChanged();
    void virtualStreamChanged();
    void deviceIndexChanged();
    void corkedChanged();

protected:
    explicit Stream(QObject *parent);
    ~Stream() override;

    quint32 m_deviceIndex;

private:
    QString m_name;
    quint32 m_clientIndex;
    bool m_virtualStream;
    bool m_corked;
};

} // QPulseAudio

#endif // STREAM_H
