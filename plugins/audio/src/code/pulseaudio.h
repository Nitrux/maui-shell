/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>
    SPDX-FileCopyrightText: 2016 David Rosca <nowrep@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PULSEAUDIO_H
#define PULSEAUDIO_H

#include <QAbstractListModel>

#include "maps.h"

namespace QPulseAudio
{
class Context;

class AbstractModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum ItemRole {
        PulseObjectRole = Qt::UserRole + 1,
    };
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

    Q_ENUM(ItemRole)

    ~AbstractModel() override;
    QHash<int, QByteArray> roleNames() const final;
    int rowCount(const QModelIndex &parent = QModelIndex()) const final;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) final;

    Q_INVOKABLE int role(const QByteArray &roleName) const;

Q_SIGNALS:
    void countChanged();

protected:
    AbstractModel(const MapBaseQObject *map, QObject *parent);
    void initRoleNames(const QMetaObject &qobjectMetaObject);
    Context *context() const;

private Q_SLOTS:
    void propertyChanged();

private:
    void onDataAdded(int index);
    void onDataRemoved(int index);
    QMetaMethod propertyChangedMetaMethod() const;

    const MapBaseQObject *m_map;
    QHash<int, QByteArray> m_roles;
    QHash<int, int> m_objectProperties;
    QHash<int, int> m_signalIndexToProperties;

private:
    // Prevent leaf-classes from default constructing as we want to enforce
    // them passing us a context or explicit nullptrs.
    AbstractModel()
    {
    }
};

class CardModel : public AbstractModel
{
    Q_OBJECT
public:
    explicit CardModel(QObject *parent = nullptr);
};

class SinkModel : public AbstractModel
{
    Q_OBJECT
    Q_PROPERTY(QPulseAudio::Sink *defaultSink READ defaultSink NOTIFY defaultSinkChanged)
    Q_PROPERTY(QPulseAudio::Sink *preferredSink READ preferredSink NOTIFY preferredSinkChanged)
public:
    enum ItemRole {
        SortByDefaultRole = PulseObjectRole + 1,
    };
    Q_ENUM(ItemRole)

    explicit SinkModel(QObject *parent = nullptr);
    Sink *defaultSink() const;
    Sink *preferredSink() const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

Q_SIGNALS:
    void defaultSinkChanged();
    void preferredSinkChanged();

private:
    void sinkAdded(int index);
    void sinkRemoved(int index);
    void updatePreferredSink();
    Sink *findPreferredSink() const;

    Sink *m_preferredSink;
};

class SinkInputModel : public AbstractModel
{
    Q_OBJECT
public:
    explicit SinkInputModel(QObject *parent = nullptr);
};

class SourceModel : public AbstractModel
{
    Q_OBJECT
    Q_PROPERTY(QPulseAudio::Source *defaultSource READ defaultSource NOTIFY defaultSourceChanged)
public:
    enum ItemRole {
        SortByDefaultRole = PulseObjectRole + 1,
    };
    Q_ENUM(ItemRole)

    explicit SourceModel(QObject *parent = nullptr);
    Source *defaultSource() const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

Q_SIGNALS:
    void defaultSourceChanged();
};

class SourceOutputModel : public AbstractModel
{
    Q_OBJECT
public:
    explicit SourceOutputModel(QObject *parent = nullptr);
};

class StreamRestoreModel : public AbstractModel
{
    Q_OBJECT
public:
    explicit StreamRestoreModel(QObject *parent = nullptr);
};

class ModuleModel : public AbstractModel
{
    Q_OBJECT
public:
    explicit ModuleModel(QObject *parent = nullptr);
};

} // QPulseAudio

#endif // PULSEAUDIO_H
