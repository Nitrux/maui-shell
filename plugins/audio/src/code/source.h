/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef SOURCE_H
#define SOURCE_H

#include "device.h"

namespace QPulseAudio
{
class Source : public Device
{
    Q_OBJECT
public:
    explicit Source(QObject *parent);

    void update(const pa_source_info *info);
    void setVolume(qint64 volume) override;
    void setMuted(bool muted) override;
    void setActivePortIndex(quint32 port_index) override;
    void setChannelVolume(int channel, qint64 volume) override;
    void setChannelVolumes(const QVector<qint64> &volumes) override;

    bool isDefault() const override;
    void setDefault(bool enable) override;

    void switchStreams() override;
};

} // QPulseAudio

#endif // SOURCE_H
