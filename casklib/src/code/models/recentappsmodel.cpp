#include "recentappsmodel.h"
#include "code/controllers/appsdb.h"

#include <QVariant>

RecentAppsModel::RecentAppsModel(AppsDB * parent) : QAbstractListModel(parent)
  ,m_db(parent)
{

}

void RecentAppsModel::setList(const QVariantList &list)
{
    beginResetModel();
    for(const auto &item : list)
    {
        m_list << AppsDB::appInfo(item.toMap().value(QStringLiteral("name")).toString());
    }

    endResetModel();
}

void RecentAppsModel::insert(const QString &desktopFile, const int &index)
{
    beginInsertRows(QModelIndex(), index, index);
    m_list.insert(index, AppsDB::appInfo(desktopFile));
    endInsertRows();
}

int RecentAppsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return m_list.count();
}

QVariant RecentAppsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    auto map = m_list.at(index.row()).toMap();

    switch(role)
    {
    case Roles::Name: return map.value(QStringLiteral("name"));
    case Roles::Icon: return map.value(QStringLiteral("icon"));
    case Roles::Executable: return map.value(QStringLiteral("executable"));
    case Roles::Comment: return map.value(QStringLiteral("comment"));
    case Roles::Path: return map.value(QStringLiteral("path"));
    }

    return QVariant();
}

QHash<int, QByteArray> RecentAppsModel::roleNames() const
{
    return {
        {Roles::Name, "name"},
        {Roles::Icon, "icon"},
        {Roles::Executable, "executable"},
        {Roles::Comment, "comment"},
        {Roles::Path, "path"}};
}
