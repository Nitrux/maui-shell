#include "appsmodel.h"

#include <KConfig>
#include <KConfigGroup>
#include <KLocalizedString>
#include <KApplicationTrader>
#include <KService>
#include <KServiceGroup>
//#include <KToolInvocation>
#include <QColor>
#include <QDebug>
#include <QFileInfo>
#include <QModelIndex>
#include <QVariantList>


#include<KLocalizedString>

AppsModel::AppsModel(QObject *parent) : MauiList(parent)
{

}

const FMH::MODEL_LIST &AppsModel::items() const
{
    return m_data;
}

static FMH::MODEL_LIST groupData( KServiceGroup::Ptr group)
{
    FMH::MODEL_LIST res;

    if(!group->groupEntries().isEmpty())
    {
        for(const auto &s : group->groupEntries())
        {
            if (!s->noDisplay() && s->serviceEntries().length() > 0 && s->childCount() > 0)
                res << FMH::MODEL {{FMH::MODEL_KEY::COMMENT, s->comment()}, {FMH::MODEL_KEY::ICON, s->icon()}, {FMH::MODEL_KEY::LABEL, s->name()}, {FMH::MODEL_KEY::PATH, s->entryPath()}};

        }
    }

    return res;
}

static FMH::MODEL_LIST getApps()
{
    FMH::MODEL_LIST res;
    KServiceGroup::Ptr group = KServiceGroup::root();

    res = groupData(group);

    return res;
}

static FMH::MODEL_LIST getApps(const QString &groupStr, const int &limit)
{
    const QString grp = QString(groupStr).replace(QStringLiteral("/"), QStringLiteral("")) + QStringLiteral("/");
    qDebug() << "APP GROUDP" << groupStr << grp;

    int i = 0;
    if (grp.isEmpty())
        return getApps();

    FMH::MODEL_LIST res;
    auto group = new KServiceGroup(grp);
    auto list = group->serviceEntries();

    for (const auto &s : list)
    {
        if (s->noDisplay())
            continue;

        if(i>=limit)
            break;

        res << FMH::MODEL {{FMH::MODEL_KEY::ICON, s->icon()}, {FMH::MODEL_KEY::COMMENT, s->comment()}, {FMH::MODEL_KEY::LABEL, s->name()}, {FMH::MODEL_KEY::PATH, s->entryPath()}, {FMH::MODEL_KEY::EXECUTABLE, s->exec()}};
        i++;
    }

    return res;
}

void AppsModel::setList()
{
    Q_EMIT preListChanged();
    m_data.clear();

    if(m_group.isEmpty() || m_group == QStringLiteral("All"))
    {
        const auto apps = getApps();
        for(const auto &item : std::as_const(apps))
        {
            const auto apps = getApps(item[FMH::MODEL_KEY::LABEL], m_limit);

            m_data << apps;
        }
    }else
    {
        const auto apps = getApps(m_group, m_limit);
        m_data << apps;
    }

    Q_EMIT postListChanged();
    Q_EMIT this->countChanged();
}


void AppsModel::componentComplete()
{
    connect(this, &AppsModel::groupChanged, this, &AppsModel::setList);
    connect(this, &AppsModel::limitChanged, this, &AppsModel::setList);
    this->setList();
}

QString AppsModel::group() const
{
    return m_group;
}

QVariantList AppsModel::groups() const
{
    FMH::MODEL_LIST res;
    res << FMH::MODEL {{FMH::MODEL_KEY::COMMENT, i18n("All Apps")}, {FMH::MODEL_KEY::ICON, QStringLiteral("love")}, {FMH::MODEL_KEY::LABEL, i18n("All")}, {FMH::MODEL_KEY::PATH, QStringLiteral("")}};

    res << getApps();
    return FMH::toMapList(res);
}

int AppsModel::limit() const
{
    return m_limit;
}

void AppsModel::setGroup(QString group)
{
    if (m_group == group)
        return;

    m_group = group;
    Q_EMIT groupChanged();
}

void AppsModel::setLimit(int limit)
{
    if (m_limit == limit)
        return;

    m_limit = limit;
    Q_EMIT limitChanged();
}
