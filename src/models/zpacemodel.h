#ifndef ZPACEMODEL_H
#define ZPACEMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QWaylandXdgSurface>
#include <QDebug>

#include "zpace.h"

class ZpaceModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    enum Roles
    {
        Space,
        Name,
        Id,
        Count
    };
    explicit ZpaceModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void insert(QWaylandXdgSurface *surface);

    /**
     * @brief currentIndex
     * current space index
     * @return
     */
    int currentIndex() const;

    Zpace* currentSpace();

    int count() const;

public slots:
    /**
     * @brief setCurrentIndex
     * set the current space index
     * @param index
     * index of the current space being used
     */
    void setCurrentIndex(int currentIndex);

private:
    QVector<Zpace*> m_zpaces;
    int m_currentIndex = -1;
    int m_count = 0;

    void setCount();

signals:
    void currentIndexChanged(int currentIndex);

    void countChanged(int count);
};

#endif // ZPACEMODEL_H
