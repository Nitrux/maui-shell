/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>
    SPDX-FileCopyrightText: 2021 Nicolas Fella

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "speakertest.h"

#include "canberracontext.h"

QPulseAudio::Sink *SpeakerTest::sink() const
{
    return m_sink;
}

void SpeakerTest::setSink(QPulseAudio::Sink *sink)
{
    if (m_sink != sink) {
        m_sink = sink;
        Q_EMIT sinkChanged();
    }
}

void SpeakerTest::testChannel(const QString &name)
{
    auto context = QPulseAudio::CanberraContext::instance()->canberra();
    if (!context) {
        return;
    }

    ca_context_set_driver(context, "pulse");

    char dev[64];
    snprintf(dev, sizeof(dev), "%lu", (unsigned long)m_sink->index());
    ca_context_change_device(context, dev);

    QString sound_name = QStringLiteral("audio-channel-") + name;
    ca_proplist *proplist;
    ca_proplist_create(&proplist);

    ca_proplist_sets(proplist, CA_PROP_MEDIA_ROLE, "test");
    ca_proplist_sets(proplist, CA_PROP_MEDIA_NAME, name.toLatin1().constData());
    ca_proplist_sets(proplist, CA_PROP_CANBERRA_FORCE_CHANNEL, name.toLatin1().data());
    ca_proplist_sets(proplist, CA_PROP_CANBERRA_ENABLE, "1");

    ca_proplist_sets(proplist, CA_PROP_EVENT_ID, sound_name.toLatin1().data());
    if (ca_context_play_full(context, 0, proplist, nullptr, NULL) != CA_SUCCESS) {
        // Try a different sound name.
        ca_proplist_sets(proplist, CA_PROP_EVENT_ID, "audio-test-signal");
        if (ca_context_play_full(context, 0, proplist, nullptr, NULL) != CA_SUCCESS) {
            // Finaly try this... if this doesn't work, then stuff it.
            ca_proplist_sets(proplist, CA_PROP_EVENT_ID, "bell-window-system");
            ca_context_play_full(context, 0, proplist, nullptr, NULL);
        }
    }

    ca_context_change_device(context, nullptr);
    ca_proplist_destroy(proplist);
}
