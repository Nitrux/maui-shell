/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "sinkinput.h"

#include "context.h"

#include "sink.h"

namespace QPulseAudio
{
SinkInput::SinkInput(QObject *parent)
    : Stream(parent)
{
}

void SinkInput::update(const pa_sink_input_info *info)
{
    updateStream(info);
    if (m_deviceIndex != info->sink) {
        m_deviceIndex = info->sink;
        Q_EMIT deviceIndexChanged();
    }
}

void SinkInput::setDeviceIndex(quint32 deviceIndex)
{
    context()->setGenericDeviceForStream(index(), deviceIndex, &pa_context_move_sink_input_by_index);
}

void SinkInput::setVolume(qint64 volume)
{
    context()->setGenericVolume(index(), -1, volume, cvolume(), &pa_context_set_sink_input_volume);
}

void SinkInput::setMuted(bool muted)
{
    context()->setGenericMute(index(), muted, &pa_context_set_sink_input_mute);
}

void SinkInput::setChannelVolume(int channel, qint64 volume)
{
    context()->setGenericVolume(index(), channel, volume, cvolume(), &pa_context_set_sink_input_volume);
}

void SinkInput::setChannelVolumes(const QVector<qint64> &channelVolumes)
{
    context()->setGenericVolumes(index(), channelVolumes, cvolume(), &pa_context_set_sink_input_volume);
}

} // QPulseAudio
