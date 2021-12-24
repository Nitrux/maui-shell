/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "sink.h"

#include "context.h"
#include "server.h"
#include "sinkinput.h"

namespace QPulseAudio
{
Sink::Sink(QObject *parent)
    : Device(parent)
{
    connect(context()->server(), &Server::defaultSinkChanged, this, &Sink::defaultChanged);
}

Sink::~Sink()
{
}

void Sink::update(const pa_sink_info *info)
{
    updateDevice(info);
    if (m_monitorIndex != info->monitor_source) {
        m_monitorIndex = info->monitor_source;
        Q_EMIT monitorIndexChanged();
    }
}

void Sink::setVolume(qint64 volume)
{
    context()->setGenericVolume(index(), -1, volume, cvolume(), &pa_context_set_sink_volume_by_index);
}

void Sink::setMuted(bool muted)
{
    context()->setGenericMute(m_index, muted, &pa_context_set_sink_mute_by_index);
}

void Sink::setActivePortIndex(quint32 port_index)
{
    Port *port = qobject_cast<Port *>(ports().at(port_index));
    if (!port) {
        qCWarning(PLASMAPA) << "invalid port set request" << port_index;
        return;
    }
    context()->setGenericPort(index(), port->name(), &pa_context_set_sink_port_by_index);
}

void Sink::setChannelVolume(int channel, qint64 volume)
{
    context()->setGenericVolume(index(), channel, volume, cvolume(), &pa_context_set_sink_volume_by_index);
}

void Sink::setChannelVolumes(const QVector<qint64> &channelVolumes)
{
    context()->setGenericVolumes(index(), channelVolumes, cvolume(), &pa_context_set_sink_volume_by_index);
}

bool Sink::isDefault() const
{
    return context()->server()->defaultSink() == this;
}

void Sink::setDefault(bool enable)
{
    if (!isDefault() && enable) {
        context()->server()->setDefaultSink(this);
    }
}

void Sink::switchStreams()
{
    auto data = context()->sinkInputs().data();
    std::for_each(data.begin(), data.end(), [this](SinkInput *paObj) {
        paObj->setDeviceIndex(m_index);
    });
}

quint32 Sink::monitorIndex() const
{
    return m_monitorIndex;
}

} // QPulseAudio
