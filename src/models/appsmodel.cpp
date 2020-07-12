#include "appsmodel.h"

AppsModel::AppsModel(QObject *parent) : QAbstractListModel(parent)
{

}


int AppsModel::rowCount(const QModelIndex &parent) const
{
    return m_surfaces.count();
}

QVariant AppsModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
    return QVariant();

    switch(role)
    {
    case Roles::Surface: return QVariant::fromValue(m_surfaces.at(index.row()));
    default: return QVariant();
    }
}

QHash<int, QByteArray> AppsModel::roleNames() const
{
    return {{Roles::Surface, "surface"}, {Roles::Count, "count"}, {Roles::Title, "title"}};

}

void AppsModel::insert(QWaylandXdgSurface *surface)
{
    emit beginInsertRows(QModelIndex(), rowCount(QModelIndex()), rowCount(QModelIndex()));
    m_surfaces << surface;
    emit endInsertRows();
}
