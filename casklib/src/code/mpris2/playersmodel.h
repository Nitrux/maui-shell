#pragma once
#include <QAbstractListModel>
class Mpris2Player;
class PlayersModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged FINAL)

public:

    enum Roles
    {
        Player,
        Title,
        ServiceName,
        IconName
    };

    PlayersModel(QObject *parent);
~PlayersModel();

    int rowCount(const QModelIndex &parent) const override final;
    QVariant data(const QModelIndex &index, int role) const override final;
    QHash<int, QByteArray> roleNames() const override final;

    void append(Mpris2Player* player);
    void remove(Mpris2Player* player);
    void remove(const int &index);
    Mpris2Player* player(const int &index);

    int indexOf(Mpris2Player* player) const;
    int indexOf(const QString & serviceName) const;

    int count() const;


private:
    QVector<Mpris2Player*> m_players;

    bool indexIsValid(const int &index) const;


Q_SIGNALS:
    void countChanged();

};
