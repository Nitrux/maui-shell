#pragma once

#include <QObject>
#include "code/models/recentappsmodel.h"
#include <QVariantList>

class DB;
class AppsDB : public QObject
{
    Q_OBJECT
    Q_PROPERTY(RecentAppsModel* recentApps READ recentApps FINAL CONSTANT)
public:
    explicit AppsDB(QObject *parent = nullptr);

    RecentAppsModel *recentApps() const;
    QVariantList recentAppsList();
    static QVariantMap appInfo(const QString &desktopFile);

public slots:
    void addRecentApp(const QString &desktopFile);
    void launchApp(const QString &desktopFile);
    void countUpApp(const QString &desktopFile);

    void createGroup(const QString &name);
    void addAppToGroup(const QString &desktopFile, const QString &groupName);

private:
    DB *m_db;
    RecentAppsModel* m_recentApps;

    const QVariantList get(const QString &queryTxt, std::function<bool(QVariantMap &item)> modifier = nullptr);


signals:

};

