/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PULSEOBJECT_H
#define PULSEOBJECT_H

#include "debug.h"
#include <QObject>

#include <pulse/introspect.h>

namespace QPulseAudio
{
class Context;

class PulseObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(quint32 index READ index CONSTANT)
    Q_PROPERTY(QString iconName READ iconName CONSTANT)
    Q_PROPERTY(QVariantMap properties READ properties NOTIFY propertiesChanged)
public:
    template<typename PAInfo>
    void updatePulseObject(PAInfo *info)
    {
        m_index = info->index;

        QVariantMap properties;
        void *it = nullptr;
        while (const char *key = pa_proplist_iterate(info->proplist, &it)) {
            Q_ASSERT(key);
            const char *value = pa_proplist_gets(info->proplist, key);
            if (!value) {
                qCDebug(PLASMAPA) << "property" << key << "not a string";
                continue;
            }
            Q_ASSERT(value);
            properties.insert(QString::fromUtf8(key), QString::fromUtf8(value));
        }

        if (m_properties != properties) {
            m_properties = properties;
            Q_EMIT propertiesChanged();
        }
    }

    quint32 index() const;
    QString iconName() const;
    QVariantMap properties() const;

Q_SIGNALS:
    void propertiesChanged();

protected:
    explicit PulseObject(QObject *parent);
    ~PulseObject() override;

    Context *context() const;
    quint32 m_index;
    QVariantMap m_properties;

private:
    // Ensure that we get properly parented.
    PulseObject();
};

} // QPulseAudio

#endif // PULSEOBJECT_H
