#ifndef DATAMODEL_H
#define DATAMODEL_H

#include <QAbstractItemModel>
#include <QJSValue>
#include <QRegExp>
#include <QSortFilterProxyModel>
#include <QVector>

class SortFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
    /**
     * The source model of this sorting proxy model. It has to inherit QAbstractItemModel (ListModel is not supported)
     */
    Q_PROPERTY(QAbstractItemModel *sourceModel READ sourceModel WRITE setModel NOTIFY sourceModelChanged)

    /**
     * The regular expression for the filter, only items with their filterRole matching filterRegExp will be displayed
     */
    Q_PROPERTY(QString filterRegExp READ filterRegExp WRITE setFilterRegExp NOTIFY filterRegExpChanged)

    /**
     * The string for the filter, only items with their filterRole matching filterString will be displayed
     */
    Q_PROPERTY(QString filterString READ filterString WRITE setFilterString NOTIFY filterStringChanged REVISION 1)

    /**
     * A JavaScript callable that is passed the source model row index as first argument and the value
     * of filterRole as second argument. The callable's return value is evaluated as boolean to determine
     * whether the row is accepted (true) or filtered out (false). It overrides the default implementation
     * that uses filterRegExp or filterString; while filterCallable is set those two properties are
     * ignored. Attempts to write a non-callable to this property are silently ignored, but you can set
     * it to null.
     */
    Q_PROPERTY(QJSValue filterCallback READ filterCallback WRITE setFilterCallback NOTIFY filterCallbackChanged REVISION 1)

    /**
     * The role of the sourceModel on which filterRegExp must be applied.
     */
    Q_PROPERTY(QString filterRole READ filterRole WRITE setFilterRole)

    /**
     * The role of the sourceModel that will be used for sorting. if empty the order will be left unaltered
     */
    Q_PROPERTY(QString sortRole READ sortRole WRITE setSortRole)

    /**
     * One of Qt.Ascending or Qt.Descending
     */
    Q_PROPERTY(Qt::SortOrder sortOrder READ sortOrder WRITE setSortOrder)

    /**
     * Specify which column should be used for sorting
     */
    Q_PROPERTY(int sortColumn READ sortColumn WRITE setSortColumn NOTIFY sortColumnChanged)

    /**
     * How many items are in this model
     */
    Q_PROPERTY(int count READ count NOTIFY countChanged)

    friend class DataModel;

public:
    explicit SortFilterModel(QObject *parent = nullptr);
    ~SortFilterModel() override;

    void setModel(QAbstractItemModel *source);

    void setFilterRegExp(const QString &exp);
    QString filterRegExp() const;

    void setFilterString(const QString &filterString);
    QString filterString() const;

    void setFilterCallback(const QJSValue &callback);
    QJSValue filterCallback() const;

    void setFilterRole(const QString &role);
    QString filterRole() const;

    void setSortRole(const QString &role);
    QString sortRole() const;

    void setSortOrder(const Qt::SortOrder order);

    void setSortColumn(int column);

    int count() const
    {
        return QSortFilterProxyModel::rowCount();
    }

    /**
     * Returns the item at index in the list model.
     * This allows the item data to be accessed (but not modified) from JavaScript.
     * It returns an Object with a property for each role.
     *
     * @param i the row we want
     */
    Q_INVOKABLE QVariantMap get(int i) const;

    Q_INVOKABLE int mapRowToSource(int i) const;

    Q_INVOKABLE int mapRowFromSource(int i) const;

Q_SIGNALS:
    void countChanged();
    void sortColumnChanged();
    void sourceModelChanged(QObject *);
    void filterRegExpChanged(const QString &);
    Q_REVISION(1) void filterStringChanged(const QString &);
    Q_REVISION(1) void filterCallbackChanged(const QJSValue &);

protected:
    int roleNameToId(const QString &name) const;
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
    QHash<int, QByteArray> roleNames() const override;

protected Q_SLOTS:
    void syncRoleNames();

private:
    QString m_filterRole;
    QString m_sortRole;
    QString m_filterString;
    QJSValue m_filterCallback;
    QHash<QString, int> m_roleIds;
};

#endif