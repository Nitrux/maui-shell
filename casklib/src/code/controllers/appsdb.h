#pragma once

#include <QObject>
#include "code/models/recentappsmodel.h"
#include "code/processlauncher.h"

#include <QVariantList>

class DB;
class WaylandProcessLauncher;
class AppsDB : public QObject
{
    Q_OBJECT
    Q_PROPERTY(RecentAppsModel* recentApps READ recentApps FINAL CONSTANT)
    Q_PROPERTY(WaylandProcessLauncher* m_launcher READ processLauncher FINAL CONSTANT)

public:
    explicit AppsDB(QObject *parent = nullptr);

    RecentAppsModel *recentApps() const;
    QVariantList recentAppsList();
    static QVariantMap appInfo(const QString &desktopFile);


    WaylandProcessLauncher* processLauncher() const;

public Q_SLOTS:
    void addRecentApp(const QString &desktopFile);
    void launchApp(const QString &desktopFile);
    void countUpApp(const QString &desktopFile);

    void createGroup(const QString &name);
    void addAppToGroup(const QString &desktopFile, const QString &groupName);

private:
    DB *m_db;
    RecentAppsModel* m_recentApps;
    WaylandProcessLauncher *m_launcher;

    const QVariantList get(const QString &queryTxt, std::function<bool(QVariantMap &item)> modifier = nullptr);
};

