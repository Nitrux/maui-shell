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
    if(!index.isValid())
        return QVariant();

    qDebug() << "trying to get a zpace" << m_zpaces.at(index.row())->appsModel()->rowCount(QModelIndex());

    switch(role)
    {
    case Roles::Space: return QVariant::fromValue(m_zpaces.at(index.row()));
    default: return QVariant();
    }
}

QHash<int, QByteArray> ZpaceModel::roleNames() const
{
    return {{Roles::Space, "space"}, {Roles::Count, "count"}, {Roles::Name, "name"}};
}

void ZpaceModel::insert(QWaylandXdgSurface *surface)
{
    qDebug()<< "Trying ot insert a surface, so need to check all the avaliable spaces and where the "
               "surface can fit, or create a new zpace." << surface->windowGeometry();

    if(m_zpaces.isEmpty())
    {
        auto zpace_ = new Zpace(this);
        if(zpace_->insert(surface))
       {
            emit beginInsertRows(QModelIndex(), rowCount(QModelIndex()), rowCount(QModelIndex()));
            m_zpaces << zpace_;
            emit endInsertRows();

            setCount();
            setCurrentIndex(count()-1);
        }

        qDebug() << "Space count << " << m_zpaces.count();
    }else
    {
        qDebug() << "Current space" << m_currentIndex;
        if(currentSpace())
        {
            if(currentSpace()->insert(surface))
           {
                qDebug() << "Surface inserted on the current space";
            }else
            {
                auto zpace_ = new Zpace(this);
                if(zpace_->insert(surface))
               {
                    emit beginInsertRows(QModelIndex(), m_currentIndex+1, m_currentIndex+1);
                    m_zpaces.insert(m_currentIndex+1, zpace_);
                    emit endInsertRows();

                    setCount();
                    setCurrentIndex(m_currentIndex+1);
                }
            }
        }

    }

//    connect(surface, &QWaylandXdgSurface::toplevelCreated, [&]()
//    {
//       qDebug() << "Surface toplevel created" << surface->windowGeometry() <<  " title <<" << surface->toplevel()->title();
    //    });
}

int ZpaceModel::currentIndex() const
{
    return m_currentIndex;
}

Zpace *ZpaceModel::currentSpace()
{
    if(m_currentIndex > count()-1 || m_currentIndex < 0)
        return nullptr;

    return m_zpaces.at(m_currentIndex);

}

int ZpaceModel::count() const
{
    return m_count;
}

void ZpaceModel::setCurrentIndex(int currentIndex)
{
    qDebug()<< "Setting currentIndex" << currentIndex;
    if(currentIndex > count()-1 || currentIndex < 0)
        return;

    if (m_currentIndex == currentIndex)
        return;

    m_currentIndex = currentIndex;
    emit currentIndexChanged(m_currentIndex);
}

void ZpaceModel::setCount()
{
    m_count = rowCount(QModelIndex());
    emit countChanged(m_count);
}

