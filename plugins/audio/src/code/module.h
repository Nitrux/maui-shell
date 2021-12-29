/*
    SPDX-FileCopyrightText: 2017 David Rosca <nowrep@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef MODULE_H
#define MODULE_H

#include <pulse/introspect.h>

#include <QMap>
#include <QVariant>

#include "pulseobject.h"

namespace QPulseAudio
{
class Module : public PulseObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString argument READ argument NOTIFY argumentChanged)

public:
    explicit Module(QObject *parent);

    void update(const pa_module_info *info);

    QString name() const;
    QString argument() const;

Q_SIGNALS:
    void nameChanged();
    void argumentChanged();

private:
    QString m_name;
    QString m_argument;
};

} // QPulseAudio

#endif // MODULE_H
