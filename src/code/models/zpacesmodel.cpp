#include "zpacesmodel.h"
#include <QDebug>
#include "code/controllers/zpaces.h"
#include "code/controllers/zpace.h"

ZpacesModel::ZpacesModel(Zpaces *parent) : QAbstractListModel(parent)
  ,m_zpacesRoot (parent)
{
    this->addZpace();
}

ZpacesModel::~ZpacesModel()
{

    qDeleteAll(m_zpacesList);
//    for(auto zpace : m_zpacesList)
//    {
//        zpace->deleteLater();
//    }
}

int ZpacesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return m_zpacesList.count();
}

QVariant ZpacesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch(role)
    {
    case Roles::ZpaceItem: return QVariant::fromValue(m_zpacesList[index.row()]);
    default: return QVariant();
    }

    return QVariant();
}

QHash<int, QByteArray> ZpacesModel::roleNames() const
{
    return QHash<int, QByteArray> {{Roles::ZpaceItem, "Zpace" }};
}

Zpace *ZpacesModel::zpace(const int &index)
{
    if(!indexIsValid(index))
        return nullptr;

    return m_zpacesList[index];
}

Zpace *ZpacesModel::addZpace()
{

    auto newZpace = createZpace();// this is not the parent but a reference to the model

    const auto index = m_zpacesList.size();

    this->beginInsertRows(QModelIndex(), index, index);
    m_zpacesList.append(newZpace);
    this->endInsertRows();

    Q_EMIT this->countChanged();
    Q_EMIT this->zpaceAdded(index);


    return newZpace;
}

Zpace *ZpacesModel::insertZpace(const int &index)
{
    if(index < 0 || index > m_zpacesList.count())
        return nullptr;

    auto newZpace = createZpace();
    this->beginInsertRows(QModelIndex(), index, index);
    m_zpacesList.insert(index, newZpace);
    this->endInsertRows();
    Q_EMIT this->countChanged();
    Q_EMIT this->zpaceAdded(index);

    return newZpace;
}

void ZpacesModel::removeZpace(const int &index)
{
    if(!indexIsValid(index))
        return;

    Zpace *zpace = m_zpacesList.at(index);

    this->beginRemoveRows(QModelIndex(), index, index);
    m_zpacesList.remove(index);
    this->endRemoveRows();
    Q_EMIT this->countChanged();

    if(!zpace)
        return;

    // Move the windows to a neighbouring zpace; close them when there is none.
    Zpace *target = nullptr;
    if(!m_zpacesList.isEmpty())
        target = m_zpacesList.at(qMin(index, m_zpacesList.size() - 1));

    const auto windows = zpace->windows()->windows();
    for(auto *window : windows)
    {
        if(!window)
            continue;

        if(target && target->acceptsWindow(window))
        {
            zpace->popWindow(window);
            target->addWindow(window);
        }else
        {
            window->close();
        }
    }

    zpace->deleteLater();
}

void ZpacesModel::moveZpace(const int &from, const int &to)
{
    if(!indexIsValid(from))
        return;

    if(to >= m_zpacesList.size() || to < 0)
        return;

    this->beginMoveRows(QModelIndex(), from, from, QModelIndex(), from < to ? to+1 : to);
    m_zpacesList.move(from, to);
    this->endMoveRows();
}

QVector<Zpace *> ZpacesModel::zpaces() const
{
    return m_zpacesList;
}

int ZpacesModel::indexOf(Zpace *zpace)
{
    if(!zpace)
        return -1;

    return m_zpacesList.indexOf(zpace);
}

bool ZpacesModel::zpaceExists(Zpace *zpace)
{
    return indexOf(zpace)>= 0;
}

int ZpacesModel::count() const
{
    return m_zpacesList.size();
}

Zpaces *ZpacesModel::zpacesRoot() const
{
    return this->m_zpacesRoot;
}

bool ZpacesModel::indexIsValid(const int &index) const
{
    if(index >= m_zpacesList.size() || index < 0)
        return false;

    return true;
}

Zpace *ZpacesModel::createZpace()
{
    auto newZpace = new Zpace(this);
    connect(newZpace, &Zpace::closed, this, [this, newZpace]
    {
       this->removeZpace(indexOf(newZpace));
    });

    return newZpace;
}
