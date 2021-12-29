/*
    SPDX-FileCopyrightText: 2016 David Rosca <nowrep@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "server.h"
#include "context.h"
#include "debug.h"
#include "sink.h"
#include "source.h"

namespace QPulseAudio
{
Server::Server(Context *context)
    : QObject(context)
    , m_defaultSink(nullptr)
    , m_defaultSource(nullptr)
    , m_isPipeWire(false)
{
    Q_ASSERT(context);

    connect(&context->sinks(), &MapBaseQObject::added, this, &Server::updateDefaultDevices);
    connect(&context->sinks(), &MapBaseQObject::removed, this, &Server::updateDefaultDevices);
    connect(&context->sources(), &MapBaseQObject::added, this, &Server::updateDefaultDevices);
    connect(&context->sources(), &MapBaseQObject::removed, this, &Server::updateDefaultDevices);
}

Sink *Server::defaultSink() const
{
    return m_defaultSink;
}

void Server::setDefaultSink(Sink *sink)
{
    Q_ASSERT(sink);
    Context::instance()->setDefaultSink(sink->name());
}

Source *Server::defaultSource() const
{
    return m_defaultSource;
}

void Server::setDefaultSource(Source *source)
{
    Q_ASSERT(source);
    Context::instance()->setDefaultSource(source->name());
}

void Server::reset()
{
    if (m_defaultSink) {
        m_defaultSink = nullptr;
        Q_EMIT defaultSinkChanged(m_defaultSink);
    }

    if (m_defaultSource) {
        m_defaultSource = nullptr;
        Q_EMIT defaultSourceChanged(m_defaultSource);
    }
}

void Server::update(const pa_server_info *info)
{
    m_defaultSinkName = QString::fromUtf8(info->default_sink_name);
    m_defaultSourceName = QString::fromUtf8(info->default_source_name);
    m_isPipeWire = QString::fromUtf8(info->server_name).contains("PipeWire");

    updateDefaultDevices();

    Q_EMIT updated();
}

template<typename Type, typename Map>
static Type *findByName(const Map &map, const QString &name)
{
    Type *out = nullptr;
    if (name.isEmpty()) {
        return out;
    }
    QMapIterator<quint32, Type *> it(map);
    while (it.hasNext()) {
        it.next();
        out = it.value();
        if (out->name() == name) {
            return out;
        }
    }
    qCWarning(PLASMAPA) << "No object for name" << name;
    return out;
}

void Server::updateDefaultDevices()
{
    Sink *sink = findByName<Sink>(Context::instance()->sinks().data(), m_defaultSinkName);
    Source *source = findByName<Source>(Context::instance()->sources().data(), m_defaultSourceName);

    if (m_defaultSink != sink) {
        qCDebug(PLASMAPA) << "Default sink changed" << sink;
        m_defaultSink = sink;
        Q_EMIT defaultSinkChanged(m_defaultSink);
    }

    if (m_defaultSource != source) {
        qCDebug(PLASMAPA) << "Default source changed" << source;
        m_defaultSource = source;
        Q_EMIT defaultSourceChanged(m_defaultSource);
    }
}

bool Server::isPipeWire() const
{
    return m_isPipeWire;
}

} // QPulseAudio
