/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>
    SPDX-FileCopyrightText: 2016 David Rosca <nowrep@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "pulseaudio.h"

#include "card.h"
#include "debug.h"
#include "module.h"
#include "server.h"
#include "sink.h"
#include "sinkinput.h"
#include "source.h"
#include "sourceoutput.h"
#include "streamrestore.h"

#include <QMetaEnum>

namespace QPulseAudio
{
AbstractModel::AbstractModel(const MapBaseQObject *map, QObject *parent)
    : QAbstractListModel(parent)
    , m_map(map)
{
    Context::instance()->ref();

    connect(m_map, &MapBaseQObject::aboutToBeAdded, this, [this](int index) {
        beginInsertRows(QModelIndex(), index, index);
    });
    connect(m_map, &MapBaseQObject::added, this, [this](int index) {
        onDataAdded(index);
        endInsertRows();
        Q_EMIT countChanged();
    });
    connect(m_map, &MapBaseQObject::aboutToBeRemoved, this, [this](int index) {
        beginRemoveRows(QModelIndex(), index, index);
    });
    connect(m_map, &MapBaseQObject::removed, this, [this](int index) {
        Q_UNUSED(index);
        endRemoveRows();
        Q_EMIT countChanged();
    });
}

AbstractModel::~AbstractModel()
{
    // deref context after we've deleted this object
    // see https://bugs.kde.org/show_bug.cgi?id=371215
    Context::instance()->unref();
}

QHash<int, QByteArray> AbstractModel::roleNames() const
{
    if (!m_roles.empty()) {
        qCDebug(PLASMAPA) << "returning roles" << m_roles;
        return m_roles;
    }
    Q_UNREACHABLE();
    return QHash<int, QByteArray>();
}

int AbstractModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return m_map->count();
}

QVariant AbstractModel::data(const QModelIndex &index, int role) const
{
    if (!hasIndex(index.row(), index.column())) {
        return QVariant();
    }
    QObject *data = m_map->objectAt(index.row());
    Q_ASSERT(data);
    if (role == PulseObjectRole) {
        return QVariant::fromValue(data);
    } else if (role == Qt::DisplayRole) {
        return static_cast<PulseObject *>(data)->properties().value(QStringLiteral("name")).toString();
    }
    int property = m_objectProperties.value(role, -1);
    if (property == -1) {
        return QVariant();
    }
    return data->metaObject()->property(property).read(data);
}

bool AbstractModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!hasIndex(index.row(), index.column())) {
        return false;
    }
    int propertyIndex = m_objectProperties.value(role, -1);
    if (propertyIndex == -1) {
        return false;
    }
    QObject *data = m_map->objectAt(index.row());
    auto property = data->metaObject()->property(propertyIndex);
    return property.write(data, value);
}

int AbstractModel::role(const QByteArray &roleName) const
{
    qCDebug(PLASMAPA) << roleName << m_roles.key(roleName, -1);
    return m_roles.key(roleName, -1);
}

Context *AbstractModel::context() const
{
    return Context::instance();
}

void AbstractModel::initRoleNames(const QMetaObject &qobjectMetaObject)
{
    m_roles[PulseObjectRole] = QByteArrayLiteral("PulseObject");

    QMetaEnum enumerator;
    for (int i = 0; i < metaObject()->enumeratorCount(); ++i) {
        if (metaObject()->enumerator(i).name() == QLatin1String("ItemRole")) {
            enumerator = metaObject()->enumerator(i);
            break;
        }
    }

    for (int i = 0; i < enumerator.keyCount(); ++i) {
        // Clip the Role suffix and glue it in the hash.
        const int roleLength = 4;
        QByteArray key(enumerator.key(i));
        // Enum values must end in Role or the enum is crap
        Q_ASSERT(key.right(roleLength) == QByteArrayLiteral("Role"));
        key.chop(roleLength);
        m_roles[enumerator.value(i)] = key;
    }

    int maxEnumValue = -1;
    for (auto it = m_roles.constBegin(); it != m_roles.constEnd(); ++it) {
        if (it.key() > maxEnumValue) {
            maxEnumValue = it.key();
        }
    }
    Q_ASSERT(maxEnumValue != -1);
    auto mo = qobjectMetaObject;
    for (int i = 0; i < mo.propertyCount(); ++i) {
        QMetaProperty property = mo.property(i);
        QString name(property.name());
        name.replace(0, 1, name.at(0).toUpper());
        m_roles[++maxEnumValue] = name.toLatin1();
        m_objectProperties.insert(maxEnumValue, i);
        if (!property.hasNotifySignal()) {
            continue;
        }
        m_signalIndexToProperties.insert(property.notifySignalIndex(), i);
    }
    qCDebug(PLASMAPA) << m_roles;

    // Connect to property changes also with objects already in model
    for (int i = 0; i < m_map->count(); ++i) {
        onDataAdded(i);
    }
}

void AbstractModel::propertyChanged()
{
    if (!sender() || senderSignalIndex() == -1) {
        return;
    }
    int propertyIndex = m_signalIndexToProperties.value(senderSignalIndex(), -1);
    if (propertyIndex == -1) {
        return;
    }
    int role = m_objectProperties.key(propertyIndex, -1);
    if (role == -1) {
        return;
    }
    int index = m_map->indexOfObject(sender());
    qCDebug(PLASMAPA) << "PROPERTY CHANGED (" << index << ") :: " << role << roleNames().value(role);
    Q_EMIT dataChanged(createIndex(index, 0), createIndex(index, 0), {role});
}

void AbstractModel::onDataAdded(int index)
{
    QObject *data = m_map->objectAt(index);
    const QMetaObject *mo = data->metaObject();
    // We have all the data changed notify signals already stored
    auto keys = m_signalIndexToProperties.keys();
    Q_FOREACH (int index, keys) {
        QMetaMethod meth = mo->method(index);
        connect(data, meth, this, propertyChangedMetaMethod());
    }
}

QMetaMethod AbstractModel::propertyChangedMetaMethod() const
{
    auto mo = metaObject();
    int methodIndex = mo->indexOfMethod("propertyChanged()");
    if (methodIndex == -1) {
        return QMetaMethod();
    }
    return mo->method(methodIndex);
}

SinkModel::SinkModel(QObject *parent)
    : AbstractModel(&context()->sinks(), parent)
    , m_preferredSink(nullptr)
{
    initRoleNames(Sink::staticMetaObject);

    for (int i = 0; i < context()->sinks().count(); ++i) {
        sinkAdded(i);
    }

    connect(&context()->sinks(), &MapBaseQObject::added, this, &SinkModel::sinkAdded);
    connect(&context()->sinks(), &MapBaseQObject::removed, this, &SinkModel::sinkRemoved);

    connect(context()->server(), &Server::defaultSinkChanged, this, [this]() {
        updatePreferredSink();
        Q_EMIT defaultSinkChanged();
    });
}

Sink *SinkModel::defaultSink() const
{
    return context()->server()->defaultSink();
}

Sink *SinkModel::preferredSink() const
{
    return m_preferredSink;
}

QVariant SinkModel::data(const QModelIndex &index, int role) const
{
    if (role == SortByDefaultRole) {
        // Workaround QTBUG-1548
        const QString pulseIndex = data(index, AbstractModel::role(QByteArrayLiteral("Index"))).toString();
        const QString defaultDevice = data(index, AbstractModel::role(QByteArrayLiteral("Default"))).toString();
        return defaultDevice + pulseIndex;
    }
    return AbstractModel::data(index, role);
}

void SinkModel::sinkAdded(int index)
{
    Q_ASSERT(qobject_cast<Sink *>(context()->sinks().objectAt(index)));
    Sink *sink = static_cast<Sink *>(context()->sinks().objectAt(index));
    connect(sink, &Sink::stateChanged, this, &SinkModel::updatePreferredSink);

    updatePreferredSink();
}

void SinkModel::sinkRemoved(int index)
{
    Q_UNUSED(index);

    updatePreferredSink();
}

void SinkModel::updatePreferredSink()
{
    Sink *sink = findPreferredSink();

    if (sink != m_preferredSink) {
        qCDebug(PLASMAPA) << "Changing preferred sink to" << sink << (sink ? sink->name() : "");
        m_preferredSink = sink;
        Q_EMIT preferredSinkChanged();
    }
}

Sink *SinkModel::findPreferredSink() const
{
    const auto &sinks = context()->sinks();

    // Only one sink is the preferred one
    if (sinks.count() == 1) {
        return static_cast<Sink *>(sinks.objectAt(0));
    }

    auto lookForState = [this](Device::State state) {
        Sink *ret = nullptr;
        QMapIterator<quint32, Sink *> it(context()->sinks().data());
        while (it.hasNext()) {
            it.next();
            if ((it.value()->isVirtualDevice() && !it.value()->isDefault()) || it.value()->state() != state) {
                continue;
            }
            if (!ret) {
                ret = it.value();
            } else if (it.value() == defaultSink()) {
                ret = it.value();
                break;
            }
        }
        return ret;
    };

    Sink *preferred = nullptr;

    // Look for playing sinks + prefer default sink
    preferred = lookForState(Device::RunningState);
    if (preferred) {
        return preferred;
    }

    // Look for idle sinks + prefer default sink
    preferred = lookForState(Device::IdleState);
    if (preferred) {
        return preferred;
    }

    // Fallback to default sink
    return defaultSink();
}

SourceModel::SourceModel(QObject *parent)
    : AbstractModel(&context()->sources(), parent)
{
    initRoleNames(Source::staticMetaObject);

    connect(context()->server(), &Server::defaultSourceChanged, this, &SourceModel::defaultSourceChanged);
}

Source *SourceModel::defaultSource() const
{
    return context()->server()->defaultSource();
}

QVariant SourceModel::data(const QModelIndex &index, int role) const
{
    if (role == SortByDefaultRole) {
        // Workaround QTBUG-1548
        const QString pulseIndex = data(index, AbstractModel::role(QByteArrayLiteral("Index"))).toString();
        const QString defaultDevice = data(index, AbstractModel::role(QByteArrayLiteral("Default"))).toString();
        return defaultDevice + pulseIndex;
    }
    return AbstractModel::data(index, role);
}

SinkInputModel::SinkInputModel(QObject *parent)
    : AbstractModel(&context()->sinkInputs(), parent)
{
    initRoleNames(SinkInput::staticMetaObject);
}

SourceOutputModel::SourceOutputModel(QObject *parent)
    : AbstractModel(&context()->sourceOutputs(), parent)
{
    initRoleNames(SourceOutput::staticMetaObject);
}

CardModel::CardModel(QObject *parent)
    : AbstractModel(&context()->cards(), parent)
{
    initRoleNames(Card::staticMetaObject);
}

StreamRestoreModel::StreamRestoreModel(QObject *parent)
    : AbstractModel(&context()->streamRestores(), parent)
{
    initRoleNames(StreamRestore::staticMetaObject);
}

ModuleModel::ModuleModel(QObject *parent)
    : AbstractModel(&context()->modules(), parent)
{
    initRoleNames(Module::staticMetaObject);
}

} // QPulseAudio
