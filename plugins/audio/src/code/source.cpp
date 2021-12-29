/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "source.h"

#include "context.h"
#include "server.h"
#include "sourceoutput.h"

namespace QPulseAudio
{
Source::Source(QObject *parent)
    : Device(parent)
{
    connect(context()->server(), &Server::defaultSourceChanged, this, &Source::defaultChanged);
}

void Source::update(const pa_source_info *info)
{
    updateDevice(info);
}

void Source::setVolume(qint64 volume)
{
    context()->setGenericVolume(index(), -1, volume, cvolume(), &pa_context_set_source_volume_by_index);
}

void Source::setMuted(bool muted)
{
    context()->setGenericMute(index(), muted, &pa_context_set_source_mute_by_index);
}

void Source::setActivePortIndex(quint32 port_index)
{
    Port *port = qobject_cast<Port *>(ports().at(port_index));
    if (!port) {
        qCWarning(PLASMAPA) << "invalid port set request" << port_index;
        return;
    }
    context()->setGenericPort(index(), port->name(), &pa_context_set_source_port_by_index);
}

void Source::setChannelVolume(int channel, qint64 volume)
{
    context()->setGenericVolume(index(), channel, volume, cvolume(), &pa_context_set_source_volume_by_index);
}

void Source::setChannelVolumes(const QVector<qint64> &volumes)
{
    context()->setGenericVolumes(index(), volumes, cvolume(), &pa_context_set_source_volume_by_index);
}

bool Source::isDefault() const
{
    return context()->server()->defaultSource() == this;
}

void Source::setDefault(bool enable)
{
    if (!isDefault() && enable) {
        context()->server()->setDefaultSource(this);
    }
}

void Source::switchStreams()
{
    auto data = context()->sourceOutputs().data();
    std::for_each(data.begin(), data.end(), [this](SourceOutput *paObj) {
        paObj->setDeviceIndex(m_index);
    });
}

} // QPulseAudio
