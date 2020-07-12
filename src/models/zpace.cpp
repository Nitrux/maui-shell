#include "zpace.h"

Zpace::Zpace(QObject *parent) : QObject(parent)
  , m_appsModel(new AppsModel(this) )
{

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
