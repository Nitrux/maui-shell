#pragma once
#include <QObject>
#include <QAbstractListModel>

class AppsDB;
class RecentAppsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles
    {
        Name,
        Icon,
        Executable,
        Path,
        Comment
    };

   explicit RecentAppsModel(AppsDB *parent);

    void setList(const QVariantList &list);

private:
    AppsDB * m_db;
    QVariantList m_list;

public:

    void insert(const QString &desktopFile, const int &index);

    int rowCount(const QModelIndex &parent) const override final;
    QVariant data(const QModelIndex &index, int role) const override final;
    QHash<int, QByteArray> roleNames() const override final;
};

