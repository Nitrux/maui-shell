/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef CLIENT_H
#define CLIENT_H

#include <pulse/introspect.h>

#include <QMap>

#include "pulseobject.h"

namespace QPulseAudio
{
class Client : public PulseObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
public:
    explicit Client(QObject *parent);
    ~Client() override;

    void update(const pa_client_info *info);

    QString name() const;

Q_SIGNALS:
    void nameChanged();

private:
    QString m_name;
};

} // QPulseAudio

#endif // CLIENT_H
