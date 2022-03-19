#include "appsdb.h"
#include "db.h"

#include <QDateTime>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>

#include <QDebug>

#include <KService>
#include <KDesktopFile>

AppsDB::AppsDB(QObject *parent) : QObject(parent)
  ,m_db(new DB(this))
  ,m_recentApps(new RecentAppsModel(this))
  ,m_launcher(new WaylandProcessLauncher(this))
{
    m_recentApps->setList(this->recentAppsList());
}

RecentAppsModel *AppsDB::recentApps() const
{
    return m_recentApps;
}

QVariantList AppsDB::recentAppsList()
{
    return get("select * from RECENT_APPS order by count DESC");
}

void AppsDB::addRecentApp(const QString &desktopFile)
{
    if(m_db->insert("RECENT_APPS", {{"name", desktopFile}, {"adddate", QDateTime::currentDateTime().toString()}, {"count", 0}}))
    {
        m_recentApps->insert(desktopFile, 0);
    }else
    {
        countUpApp(desktopFile);
    }
}

void AppsDB::launchApp(const QString &desktopFile)
{
    auto info = AppsDB::appInfo(desktopFile);
    m_launcher->launch(info.value("executable").toString());
    this->addRecentApp(desktopFile);
}

void AppsDB::countUpApp(const QString &desktopFile)
{
    auto query = m_db->getQuery();
    query.prepare("UPDATE RECENT_APPS SET count = count + 1 WHERE name = :name");
    query.bindValue(":name", desktopFile);
    query.exec();
}

void AppsDB::createGroup(const QString &name)
{

}

void AppsDB::addAppToGroup(const QString &desktopFile, const QString &groupName)
{

}

const QVariantList AppsDB::get(const QString &queryTxt, std::function<bool(QVariantMap &item)> modifier)
{
    QVariantList mapList;

    auto query = m_db->getQuery(queryTxt);

    if (query.exec())
    {
        while (query.next())
        {
            QVariantMap data;
            auto rec = query.record();
            for(auto i = 0; i <rec.count() ; i++)
            {
                qDebug() << "APPSMODEL <<<<<<<<<<<<<<<<<<<" << rec.fieldName(i);
                data[rec.fieldName(i)] = query.value(i);
            }

            if (modifier)
            {
                if (!modifier(data))
                {
                    continue;
                }
            }

            mapList << data;
        }

    } else
    {
        qDebug() << query.lastError() << query.lastQuery();
    }

    return mapList;
}

QVariantMap AppsDB::appInfo(const QString &desktopFile)
{
    QVariantMap res;
    KDesktopFile file(desktopFile);
    KService service(&file);

    res.insert("name", service.name());
    res.insert("executable", service.exec());
    res.insert("path", service.entryPath());
    res.insert("comment", service.comment());
    res.insert("icon", service.icon());

    return res;
}

WaylandProcessLauncher *AppsDB::processLauncher() const
{
    return m_launcher;
}
