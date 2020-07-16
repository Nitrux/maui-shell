#include "zpace.h"

Zpace::Zpace(QObject *parent) : QObject(parent)
  , m_appsModel(new AppsModel(this) )
{
    connect(m_appsModel, &AppsModel::rowsRemoved, [&](QModelIndex, int, int)
    {
        emit countChanged(count());
    });

    connect(m_appsModel, &AppsModel::rowsInserted, [&](QModelIndex, int, int)
    {
        emit countChanged(count());
    });
}

AppsModel *Zpace::appsModel() const
{
    return m_appsModel;
}

bool Zpace::insert(QWaylandXdgSurface *surface)
{
    if(isFull())
    {
        return false;
    }

    m_appsModel->insert(surface);
    return true;
}

bool Zpace::isFull()
{
    return m_maxQuota == count();
}

int Zpace::maxQuota() const
{
    return m_maxQuota;
}

int Zpace::count() const
{
    return appsModel()->rowCount(QModelIndex());
}


