/*
    SPDX-FileCopyrightText: 2020 David Edmundson <davidedmundson@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once

#include <QObject>
#include <QPointer>
#include <QQmlParserStatus>

struct pa_stream;

namespace QPulseAudio
{
class VolumeObject;

/**
 * This class provides a way to see the "peak" volume currently playing of any VolumeObject
 */
class VolumeMonitor : public QObject
{
    Q_OBJECT
    /**
     * Object to monitor the volume of
     * This is the "PulseObject" role of any SinkInput, Sink or Output model
     * Setting to null will stop streaming
     */
    Q_PROPERTY(QPulseAudio::VolumeObject *target READ target WRITE setTarget NOTIFY targetChanged)
    /**
     * The peak output for the volume at any given moment
     * Value is normalised between 0 and 1
     */
    Q_PROPERTY(qreal volume MEMBER m_volume NOTIFY volumeChanged)

    /**
     * Whether monitoring is available
     */
    Q_PROPERTY(bool available READ isAvailable NOTIFY availableChanged)

public:
    VolumeMonitor(QObject *parent = nullptr);
    ~VolumeMonitor();

    bool isAvailable() const;

    VolumeObject *target() const;
    void setTarget(VolumeObject *target);

Q_SIGNALS:
    void volumeChanged();
    void targetChanged();
    void availableChanged();

private:
    void createStream();
    void updateVolume(qreal volume);
    static void read_callback(pa_stream *s, size_t length, void *userdata);
    static void suspended_callback(pa_stream *s, void *userdata);

    VolumeObject *m_target;
    pa_stream *m_stream = nullptr;

    qreal m_volume = 0;
};

}
