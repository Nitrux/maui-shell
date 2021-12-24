/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef SINK_H
#define SINK_H

#include "device.h"
#include <pulse/channelmap.h>

namespace QPulseAudio
{
class Sink : public Device
{
    Q_OBJECT
public:
    explicit Sink(QObject *parent);
    virtual ~Sink();

    void update(const pa_sink_info *info);
    void setVolume(qint64 volume) override;
    void setMuted(bool muted) override;
    void setActivePortIndex(quint32 port_index) override;
    void setChannelVolume(int channel, qint64 volume) override;
    void setChannelVolumes(const QVector<qint64> &channelVolumes) override;

    bool isDefault() const override;
    void setDefault(bool enable) override;

    void switchStreams() override;

    quint32 monitorIndex() const;

Q_SIGNALS:
    void monitorIndexChanged();

private:
    quint32 m_monitorIndex = -1;
};

} // QPulseAudio

#endif // SINK_H
