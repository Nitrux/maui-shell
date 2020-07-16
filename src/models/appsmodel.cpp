#include "appsmodel.h"
#include <QDebug>

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

	connect(surface, &QWaylandXdgSurface::surfaceChanged, []()
	{
		qDebug() << "Surface changed";
	});

//    connect(surface, &QWaylandXdgSurface::destroyed, [&]()
//	{
//        auto index = surfaceIndex(surface);


//	});

	m_surfaces << surface;
    emit endInsertRows();
}

void AppsModel::remove(const int &index)
{
    qDebug() << "Surface destroyed" << index << m_surfaces.count();

            if(index < rowCount(QModelIndex()) || index > 0)
            {
                emit beginRemoveRows (QModelIndex(), index, index);
                m_surfaces.removeAt (index);
                emit endRemoveRows ();
            }
}

int AppsModel::surfaceIndex(const QWaylandXdgSurface * surface) const
{
    return -1;
}
