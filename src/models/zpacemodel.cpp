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

	findZpace ()->insert(surface);



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

Zpace * ZpaceModel::findZpace()
{
	if(m_zpaces.isEmpty())
	{
		return createZpace (0);

		qDebug() << "Space count << " << m_zpaces.count();
	}else
	{
		qDebug() << "Current space" << m_currentIndex;
		if(currentSpace())
		{
			if(!currentSpace()->isFull ())
			{
				qDebug() << "Surface inserted on the current space";
				return currentSpace ();
			}else
			{
				//since current space if full iterate to find a empty szapce or create a new one

				auto zpace_ = std::find_if(m_zpaces.begin () + m_currentIndex, m_zpaces.end (), [](Zpace *zpace) -> bool
				{
					return !zpace->isFull ();
				});

				if(zpace_ != m_zpaces.end())
				{
					return *zpace_;
				}

				return createZpace (m_currentIndex+1);
			}
		}
	}

	return nullptr;
}

Zpace * ZpaceModel::createZpace(const int &index)
{
	auto zpace_ = new Zpace(this);

    connect(zpace_, &Zpace::countChanged, [this, z = zpace_](int count)
    {
        auto index = zpaceIndex(z);
        qDebug() << "Find index" << index;
        if(count == 0 && (index < rowCount(QModelIndex()) || index > 0))
        {
            emit beginRemoveRows(QModelIndex(), index, index);
            z->deleteLater();
            m_zpaces.removeAt(index);
            emit endRemoveRows();

            setCount();
            setCurrentIndex(m_count >= 1 ? m_currentIndex-1 : -1);
        }
    });

	emit beginInsertRows(QModelIndex(), index, index);
	m_zpaces.insert(index, zpace_);
	emit endInsertRows();

	setCount();
	setCurrentIndex(index);

	return zpace_;
}

int ZpaceModel::zpaceIndex(const Zpace * zpace) const
{
    qDebug() << zpace;

    auto it = std::find(m_zpaces.constBegin(), m_zpaces.constEnd(), zpace);
return std::distance(m_zpaces.constBegin(), it);
}

