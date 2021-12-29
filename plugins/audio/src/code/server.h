/*
    SPDX-FileCopyrightText: 2016 David Rosca <nowrep@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <pulse/introspect.h>

namespace QPulseAudio
{
class Sink;
class Source;
class Context;

class Server : public QObject
{
    Q_OBJECT

public:
    explicit Server(Context *context);

    Sink *defaultSink() const;
    void setDefaultSink(Sink *sink);

    Source *defaultSource() const;
    void setDefaultSource(Source *source);

    void reset();
    void update(const pa_server_info *info);

    bool isPipeWire() const;

Q_SIGNALS:
    void defaultSinkChanged(Sink *sink);
    void defaultSourceChanged(Source *source);
    void updated();

private:
    void updateDefaultDevices();

    QString m_defaultSinkName;
    QString m_defaultSourceName;
    Sink *m_defaultSink;
    Source *m_defaultSource;
    bool m_isPipeWire;
};

} // QPulseAudio

#endif // CONTEXT_H
