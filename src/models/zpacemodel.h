#ifndef ZPACEMODEL_H
#define ZPACEMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QWaylandXdgSurface>
#include <QDebug>

#include "appsmodel.h"
#include "zpace.h"

class ZpaceModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(AppsModel *appsModel READ appsModel CONSTANT FINAL)

public:
    enum Roles
    {
        Apps,
        Name,
        Id,
        Count
    };
    explicit ZpaceModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void insert(QWaylandXdgSurface *surface);

    AppsModel * appsModel() const
    {
        return m_appsModel;
    }

private:
    QVector<Zpace*> m_zpaces;
    AppsModel * m_appsModel;
};

#endif // ZPACEMODEL_H
