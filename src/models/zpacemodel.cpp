#include "zpacemodel.h"

ZpaceModel::ZpaceModel(QObject *parent) : QAbstractListModel(parent)
{

}

int ZpaceModel::rowCount(const QModelIndex &parent) const
{
    return m_zpaces.count();
}

QVariant ZpaceModel::data(const QModelIndex &index, int role) const
{
    return QVariant();
}

QHash<int, QByteArray> ZpaceModel::roleNames() const
{
    return {{Roles::Apps, "apps"}, {Roles::Count, "count"}, {Roles::Name, "name"}};
}

void ZpaceModel::insert(QWaylandXdgSurface *surface)
{
    qDebug()<< "Trying ot insert a surface, so need to check all the avaliable spaces and where the "
               "surface can fit, or create a new zpace." << surface->windowGeometry();

    connect(surface, &QWaylandXdgSurface::toplevelCreated, [&]()
    {
       qDebug() << "Surface toplevel created" << surface->windowGeometry() <<  " title <<" << surface->toplevel()->title();
    });
}
