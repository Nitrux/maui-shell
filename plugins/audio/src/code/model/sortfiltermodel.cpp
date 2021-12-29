#include "sortfiltermodel.h"

#include <QQmlContext>
#include <QQmlEngine>

SortFilterModel::SortFilterModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setObjectName(QStringLiteral("SortFilterModel"));
    setDynamicSortFilter(true);
    connect(this, &QAbstractItemModel::rowsInserted, this, &SortFilterModel::countChanged);
    connect(this, &QAbstractItemModel::rowsRemoved, this, &SortFilterModel::countChanged);
    connect(this, &QAbstractItemModel::modelReset, this, &SortFilterModel::countChanged);
    connect(this, &SortFilterModel::countChanged, this, &SortFilterModel::syncRoleNames);
}

SortFilterModel::~SortFilterModel()
{
}

void SortFilterModel::syncRoleNames()
{
    if (!sourceModel()) {
        return;
    }

    m_roleIds.clear();
    const QHash<int, QByteArray> rNames = roleNames();
    m_roleIds.reserve(rNames.count());
    for (auto i = rNames.constBegin(); i != rNames.constEnd(); ++i) {
        m_roleIds[QString::fromUtf8(i.value())] = i.key();
    }

    setFilterRole(m_filterRole);
    setSortRole(m_sortRole);
}

QHash<int, QByteArray> SortFilterModel::roleNames() const
{
    if (sourceModel()) {
        return sourceModel()->roleNames();
    }
    return {};
}

int SortFilterModel::roleNameToId(const QString &name) const
{
    return m_roleIds.value(name, Qt::DisplayRole);
}

void SortFilterModel::setModel(QAbstractItemModel *model)
{
    if (model == sourceModel()) {
        return;
    }

    if (sourceModel()) {
        disconnect(sourceModel(), &QAbstractItemModel::modelReset, this, &SortFilterModel::syncRoleNames);
    }

    QSortFilterProxyModel::setSourceModel(model);

    if (model) {
        connect(model, &QAbstractItemModel::modelReset, this, &SortFilterModel::syncRoleNames);
        syncRoleNames();
    }

    Q_EMIT sourceModelChanged(model);
}

bool SortFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (m_filterCallback.isCallable()) {
        QJSValueList args;
        args << QJSValue(source_row);

        const QModelIndex idx = sourceModel()->index(source_row, filterKeyColumn(), source_parent);
        QQmlEngine *engine = QQmlEngine::contextForObject(this)->engine();
        args << engine->toScriptValue<QVariant>(idx.data(m_roleIds.value(m_filterRole)));

        return const_cast<SortFilterModel *>(this)->m_filterCallback.call(args).toBool();
    }

    return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}

void SortFilterModel::setFilterRegExp(const QString &exp)
{
    if (exp == filterRegExp()) {
        return;
    }
    QSortFilterProxyModel::setFilterRegExp(QRegExp(exp, Qt::CaseInsensitive));
    Q_EMIT filterRegExpChanged(exp);
}

QString SortFilterModel::filterRegExp() const
{
    return QSortFilterProxyModel::filterRegExp().pattern();
}

void SortFilterModel::setFilterString(const QString &filterString)
{
    if (filterString == m_filterString) {
        return;
    }
    m_filterString = filterString;
    QSortFilterProxyModel::setFilterFixedString(filterString);
    Q_EMIT filterStringChanged(filterString);
}

QString SortFilterModel::filterString() const
{
    return m_filterString;
}

QJSValue SortFilterModel::filterCallback() const
{
    return m_filterCallback;
}

void SortFilterModel::setFilterCallback(const QJSValue &callback)
{
    if (m_filterCallback.strictlyEquals(callback)) {
        return;
    }

    if (!callback.isNull() && !callback.isCallable()) {
        return;
    }

    m_filterCallback = callback;
    invalidateFilter();

    Q_EMIT filterCallbackChanged(callback);
}

void SortFilterModel::setFilterRole(const QString &role)
{
    QSortFilterProxyModel::setFilterRole(roleNameToId(role));
    m_filterRole = role;
}

QString SortFilterModel::filterRole() const
{
    return m_filterRole;
}

void SortFilterModel::setSortRole(const QString &role)
{
    m_sortRole = role;
    if (role.isEmpty()) {
        sort(-1, Qt::AscendingOrder);
    } else if (sourceModel()) {
        QSortFilterProxyModel::setSortRole(roleNameToId(role));
        sort(sortColumn(), sortOrder());
    }
}

QString SortFilterModel::sortRole() const
{
    return m_sortRole;
}

void SortFilterModel::setSortOrder(const Qt::SortOrder order)
{
    if (order == sortOrder()) {
        return;
    }
    sort(sortColumn(), order);
}

void SortFilterModel::setSortColumn(int column)
{
    if (column == sortColumn()) {
        return;
    }
    sort(column, sortOrder());
    Q_EMIT sortColumnChanged();
}

QVariantMap SortFilterModel::get(int row) const
{
    QModelIndex idx = index(row, 0);
    QVariantMap hash;

    const QHash<int, QByteArray> rNames = roleNames();
    for (auto i = rNames.begin(); i != rNames.end(); ++i) {
        hash[QString::fromUtf8(i.value())] = data(idx, i.key());
    }

    return hash;
}

int SortFilterModel::mapRowToSource(int row) const
{
    QModelIndex idx = index(row, 0);
    return mapToSource(idx).row();
}

int SortFilterModel::mapRowFromSource(int row) const
{
    if (!sourceModel()) {
        qWarning() << "No source model defined!";
        return -1;
    }
    QModelIndex idx = sourceModel()->index(row, 0);
    return mapFromSource(idx).row();
}