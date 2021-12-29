/*
    SPDX-FileCopyrightText: 2020 David Edmundson <davidedmundson@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "volumemonitor.h"

#include <pulse/pulseaudio.h>

#include "context.h"
#include "debug.h"
#include "sink.h"
#include "sinkinput.h"
#include "source.h"
#include "sourceoutput.h"
#include "volumeobject.h"

#include <QtGlobal>

using namespace QPulseAudio;

VolumeMonitor::VolumeMonitor(QObject *parent)
    : QObject(parent)
{
    Context::instance()->ref();
}

VolumeMonitor::~VolumeMonitor()
{
    setTarget(nullptr);
    Context::instance()->unref();
}

bool VolumeMonitor::isAvailable() const
{
    return m_stream != nullptr;
}

void VolumeMonitor::updateVolume(qreal volume)
{
    // qFuzzyCompare cannot compare against 0.
    if (qFuzzyCompare(1 + m_volume, 1 + volume)) {
        return;
    }

    m_volume = volume;
    Q_EMIT volumeChanged();
}

QPulseAudio::VolumeObject *QPulseAudio::VolumeMonitor::target() const
{
    return m_target;
}

void QPulseAudio::VolumeMonitor::setTarget(QPulseAudio::VolumeObject *target)
{
    if (target == m_target) {
        return;
    }

    if (m_stream) {
        pa_stream_set_read_callback(m_stream, nullptr, nullptr);
        pa_stream_set_suspended_callback(m_stream, nullptr, nullptr);
        if (pa_stream_get_state(m_stream) == PA_STREAM_CREATING) {
            pa_stream_set_state_callback(
                m_stream,
                [](pa_stream *s, void *) {
                    pa_stream_disconnect(s);
                    pa_stream_set_state_callback(s, nullptr, nullptr);
                },
                nullptr);
        } else {
            pa_stream_disconnect(m_stream);
        }
        pa_stream_unref(m_stream);
        m_stream = nullptr;
        Q_EMIT availableChanged();
    }

    m_target = target;

    if (target) {
        connect(target, &QObject::destroyed, this, [this] {
            setTarget(nullptr);
        });
        createStream();
    }

    Q_EMIT targetChanged();
}

void VolumeMonitor::createStream()
{
    Q_ASSERT(!m_stream);

    uint32_t sourceIdx = PA_INVALID_INDEX;
    uint32_t streamIdx = PA_INVALID_INDEX;

    if (auto *sinkInput = qobject_cast<SinkInput *>(m_target)) {
        Sink *sink = Context::instance()->sinks().data().value(sinkInput->deviceIndex());
        if (sink) {
            sourceIdx = sink->monitorIndex();
        }
        streamIdx = sinkInput->index();
    } else if (auto *sourceOutput = qobject_cast<SourceOutput *>(m_target)) {
        sourceIdx = sourceOutput->deviceIndex();
        streamIdx = sourceOutput->index();
    } else if (auto *sink = qobject_cast<Sink *>(m_target)) {
        sourceIdx = sink->monitorIndex();
    } else if (auto *source = qobject_cast<Source *>(m_target)) {
        sourceIdx = source->index();
    } else {
        Q_UNREACHABLE();
    }

    if (sourceIdx == PA_INVALID_INDEX) {
        return;
    }

    char t[16];
    pa_buffer_attr attr;
    pa_sample_spec ss;
    pa_stream_flags_t flags;

    ss.channels = 1;
    ss.format = PA_SAMPLE_FLOAT32;
    ss.rate = 25;

    memset(&attr, 0, sizeof(attr));
    attr.fragsize = sizeof(float);
    attr.maxlength = (uint32_t)-1;

    snprintf(t, sizeof(t), "%u", sourceIdx);

    if (!(m_stream = pa_stream_new(Context::instance()->context(), "PlasmaPA-VolumeMeter", &ss, nullptr))) {
        qCWarning(PLASMAPA) << "Failed to create stream";
        return;
    }

    if (streamIdx != PA_INVALID_INDEX) {
        pa_stream_set_monitor_stream(m_stream, streamIdx);
    }

    pa_stream_set_read_callback(m_stream, read_callback, this);
    pa_stream_set_suspended_callback(m_stream, suspended_callback, this);

    flags = (pa_stream_flags_t)(PA_STREAM_DONT_MOVE | PA_STREAM_PEAK_DETECT | PA_STREAM_ADJUST_LATENCY);

    if (pa_stream_connect_record(m_stream, t, &attr, flags) < 0) {
        pa_stream_unref(m_stream);
        m_stream = nullptr;
        return;
    }
    Q_EMIT availableChanged();
}

void VolumeMonitor::suspended_callback(pa_stream *s, void *userdata)
{
    VolumeMonitor *w = static_cast<VolumeMonitor *>(userdata);
    if (pa_stream_is_suspended(s)) {
        w->updateVolume(-1);
    }
}

void VolumeMonitor::read_callback(pa_stream *s, size_t length, void *userdata)
{
    VolumeMonitor *w = static_cast<VolumeMonitor *>(userdata);
    const void *data;
    double volume;

    if (pa_stream_peek(s, &data, &length) < 0) {
        qCWarning(PLASMAPA) << "Failed to read data from stream";
        return;
    }

    if (!data) {
        /* nullptr data means either a hole or empty buffer.
         * Only drop the stream when there is a hole (length > 0) */
        if (length) {
            pa_stream_drop(s);
        }
        return;
    }

    Q_ASSERT(length > 0);
    Q_ASSERT(length % sizeof(float) == 0);

    volume = ((const float *)data)[length / sizeof(float) - 1];

    pa_stream_drop(s);

    volume = qBound(0.0, volume, 1.0);
    w->updateVolume(volume);
}
