/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef MAPS_H
#define MAPS_H

#include "debug.h"
#include <QMap>
#include <QObject>

#include <pulse/ext-stream-restore.h>
#include <pulse/pulseaudio.h>

namespace QPulseAudio
{
// Used for typedefs.
class Card;
class Client;
class Sink;
class SinkInput;
class Source;
class SourceOutput;
class StreamRestore;
class Module;

/**
 * @see MapBase
 * This class is nothing more than the QObject base since moc cannot handle
 * templates.
 */
class MapBaseQObject : public QObject
{
    Q_OBJECT

public:
    virtual int count() const = 0;
    virtual QObject *objectAt(int index) const = 0;
    virtual int indexOfObject(QObject *object) const = 0;

Q_SIGNALS:
    void aboutToBeAdded(int index);
    void added(int index);
    void aboutToBeRemoved(int index);
    void removed(int index);
};

/**
 * Maps a specific index to a specific object pointer.
 * This is used to give the unique arbitrary PulseAudio index of a PulseObject a
 * serialized list index. Namely it enables us to translate a discrete list
 * index to a pulse index to an object, and any permutation thereof.
 */
template<typename Type, typename PAInfo>
class MapBase : public MapBaseQObject
{
public:
    ~MapBase() override
    {
    }

    const QMap<quint32, Type *> &data() const
    {
        return m_data;
    }

    int count() const override
    {
        return m_data.count();
    }

    int indexOfObject(QObject *object) const override
    {
        int index = 0;
        QMapIterator<quint32, Type *> it(m_data);
        while (it.hasNext()) {
            it.next();
            if (it.value() == object) {
                return index;
            }
            index++;
        }
        return -1;
    }

    QObject *objectAt(int index) const override
    {
        return (m_data.constBegin() + index).value();
    }

    void reset()
    {
        while (!m_data.isEmpty()) {
            removeEntry(m_data.lastKey());
        }
        m_pendingRemovals.clear();
    }

    void insert(Type *object)
    {
        Q_ASSERT(!m_data.contains(object->index()));

        int modelIndex = 0;
        for (auto it = m_data.constBegin(); it != m_data.constEnd(); ++it) {
            if (object->index() < it.key()) {
                break;
            }
            modelIndex++;
        }

        Q_EMIT aboutToBeAdded(modelIndex);
        m_data.insert(object->index(), object);
        Q_ASSERT(modelIndex == m_data.keys().indexOf(object->index()));
        Q_EMIT added(modelIndex);
    }

    // Context is passed in as parent because context needs to include the maps
    // so we'd cause a circular dep if we were to try to use the instance here.
    // Plus that's weird separation anyway.
    void updateEntry(const PAInfo *info, QObject *parent)
    {
        Q_ASSERT(info);

        if (m_pendingRemovals.remove(info->index)) {
            // Was already removed again.
            return;
        }

        auto *obj = m_data.value(info->index, nullptr);
        if (!obj) {
            obj = new Type(parent);
        }
        obj->update(info);

        if (!m_data.contains(info->index)) {
            insert(obj);
        }
    }

    void removeEntry(quint32 index)
    {
        if (!m_data.contains(index)) {
            m_pendingRemovals.insert(index);
        } else {
            const int modelIndex = m_data.keys().indexOf(index);
            Q_EMIT aboutToBeRemoved(modelIndex);
            delete m_data.take(index);
            Q_EMIT removed(modelIndex);
        }
    }

protected:
    QMap<quint32, Type *> m_data;
    QSet<quint32> m_pendingRemovals;
};

typedef MapBase<Sink, pa_sink_info> SinkMap;
typedef MapBase<SinkInput, pa_sink_input_info> SinkInputMap;
typedef MapBase<Source, pa_source_info> SourceMap;
typedef MapBase<SourceOutput, pa_source_output_info> SourceOutputMap;
typedef MapBase<Client, pa_client_info> ClientMap;
typedef MapBase<Card, pa_card_info> CardMap;
typedef MapBase<Module, pa_module_info> ModuleMap;
typedef MapBase<StreamRestore, pa_ext_stream_restore_info> StreamRestoreMap;

} // QPulseAudio

#endif // MAPS_H
