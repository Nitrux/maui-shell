#include "playersmodel.h"
#include "mpris2player.h"

PlayersModel::PlayersModel(QObject *parent) : QAbstractListModel(parent)
{

}

PlayersModel::~PlayersModel()
{
    qDeleteAll(m_players);
}


int PlayersModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return m_players.count();
}

QVariant PlayersModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    auto player = m_players[index.row()];
    switch(role)
    {
    case Roles::Title: return QVariant(player->identity());
    case Roles::ServiceName: return QVariant(player->serviceName());
    case Roles::IconName: return QVariant(player->iconName());
    case Roles::Player:
    default:
        return QVariant::fromValue(player);
    }

    return QVariant();
}

QHash<int, QByteArray> PlayersModel::roleNames() const
{
    return QHash<int, QByteArray> {{Roles::Player, "player"},
                                  {Roles::Title, "title"},
                                  {Roles::ServiceName, "serviceName"},
                                  {Roles::IconName, "iconName"}};
}

void PlayersModel::append(Mpris2Player *player)
{
    const auto index = m_players.size();

    this->beginInsertRows(QModelIndex(), index, index);
    m_players << player;
    this->endInsertRows();
    Q_EMIT this->countChanged();
}

void PlayersModel::remove(Mpris2Player *player)
{
    this->remove(indexOf(player));
}

void PlayersModel::remove(const int &index)
{
    if(!indexIsValid(index))
    {
        return;
    }

    this->beginRemoveRows(QModelIndex(), index, index);
    m_players.remove(index);
    this->endRemoveRows();
    Q_EMIT this->countChanged();
}

Mpris2Player *PlayersModel::player(const int &index)
{
    if(indexIsValid(index))
    {
        return m_players[index];
    }

    return nullptr;
}

int PlayersModel::indexOf(Mpris2Player *player) const
{
    return m_players.indexOf(player);
}

int PlayersModel::indexOf(const QString &serviceName) const
{
    for(auto i = 0; i < m_players.size(); i++)
    {
        if(m_players.at(i)->serviceName() == serviceName)
            return i;
    }

    return -1;
}

int PlayersModel::count() const
{
    return m_players.count();
}

bool PlayersModel::indexIsValid(const int &index) const
{
    if(index >= m_players.size() || index < 0)
        return false;

    return true;
}
