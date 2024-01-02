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
    return get(QStringLiteral("select * from RECENT_APPS order by count DESC"));
}

void AppsDB::addRecentApp(const QString &desktopFile)
{
    if(m_db->insert(QStringLiteral("RECENT_APPS"), {{QStringLiteral("name"), desktopFile}, {QStringLiteral("adddate"), QDateTime::currentDateTime().toString()}, {QStringLiteral("count"), 0}}))
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
    m_launcher->launch(info.value(QStringLiteral("executable")).toString());
    this->addRecentApp(desktopFile);
}

void AppsDB::countUpApp(const QString &desktopFile)
{
    auto query = m_db->getQuery();
    query.prepare(QStringLiteral("UPDATE RECENT_APPS SET count = count + 1 WHERE name = :name"));
    query.bindValue(QStringLiteral(":name"), desktopFile);
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

    res.insert(QStringLiteral("name"), service.name());
    res.insert(QStringLiteral("executable"), service.exec());
    res.insert(QStringLiteral("path"), service.entryPath());
    res.insert(QStringLiteral("comment"), service.comment());
    res.insert(QStringLiteral("icon"), service.icon());

    return res;
}

WaylandProcessLauncher *AppsDB::processLauncher() const
{
    return m_launcher;
}
