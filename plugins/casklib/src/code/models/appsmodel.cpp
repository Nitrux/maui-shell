#include "appsmodel.h"

#include <KConfig>
#include <KConfigGroup>
#include <KLocalizedString>
#include <KMimeTypeTrader>
#include <KService>
#include <KServiceGroup>
#include <KToolInvocation>
#include <QColor>
#include <QDebug>
#include <QFileInfo>
#include <QModelIndex>
#include <QVariantList>


AppsModel::AppsModel(QObject *parent) : MauiList(parent)
{

}

const FMH::MODEL_LIST &AppsModel::items() const
{
    return m_data;
}

static FMH::MODEL_LIST getApps()
{
    FMH::MODEL_LIST res;
    KServiceGroup::Ptr group = KServiceGroup::root();

    bool sortByGenericName = false;

    KServiceGroup::List list = group->entries(true /* sorted */, true /* excludeNoDisplay */, true /* allowSeparators */, sortByGenericName /* sortByGenericName */);

    for (KServiceGroup::List::ConstIterator it = list.constBegin(); it != list.constEnd(); it++) {
        const KSycocaEntry::Ptr p = (*it);

        if (p->isType(KST_KServiceGroup))
        {
            KServiceGroup::Ptr s(static_cast<KServiceGroup *>(p.data()));

            if (!s->noDisplay() && s->childCount() > 0) {
                res << FMH::MODEL {{FMH::MODEL_KEY::COMMENT, s->comment()}, {FMH::MODEL_KEY::ICON, s->icon()}, {FMH::MODEL_KEY::LABEL, s->name()}, {FMH::MODEL_KEY::PATH, s->entryPath()}};

            }
        }
    }
    return res;
}

static FMH::MODEL_LIST getApps(const QString &groupStr, const int &limit)
{
    const auto grp = QString(groupStr).replace("/", "") + "/";
    qDebug() << "APP GROUDP" << groupStr << grp;

    int i = 0;
    if (grp.isEmpty())
        return getApps();

    FMH::MODEL_LIST res;
    //    const KServiceGroup::Ptr group(static_cast<KServiceGroup*>(groupStr));
    auto group = new KServiceGroup(grp);
    KServiceGroup::List list = group->entries(true /* sorted */, true /* excludeNoDisplay */, false /* allowSeparators */, true /* sortByGenericName */);

    for (KServiceGroup::List::ConstIterator it = list.constBegin(); it != list.constEnd(); it++) {
        const KSycocaEntry::Ptr p = (*it);

        if (p->isType(KST_KService))
        {
            const KService::Ptr s(static_cast<KService *>(p.data()));

            if (s->noDisplay())
                continue;

            if(i>=limit)
                break;

             res << FMH::MODEL {{FMH::MODEL_KEY::ICON, s->icon()}, {FMH::MODEL_KEY::COMMENT, s->comment()}, {FMH::MODEL_KEY::LABEL, s->name()}, {FMH::MODEL_KEY::PATH, s->entryPath()}};
             i++;

        } else if (p->isType(KST_KServiceSeparator))
        {
            qDebug() << "separator wtf";

        } else if (p->isType(KST_KServiceGroup))
        {
            const KServiceGroup::Ptr s(static_cast<KServiceGroup *>(p.data()));

            if(i>=limit)
                break;

            if (s->childCount() == 0)
                continue;
            res << FMH::MODEL {{FMH::MODEL_KEY::ICON, s->icon()}, {FMH::MODEL_KEY::CATEGORY, s->baseGroupName()}, {FMH::MODEL_KEY::MESSAGE, s->caption()}, {FMH::MODEL_KEY::COMMENT, s->comment()}, {FMH::MODEL_KEY::LABEL, s->name()}, {FMH::MODEL_KEY::PATH, s->entryPath()}};

            i++;

        }
    }

    return res;
}

void AppsModel::setList()
{
    emit preListChanged();
    m_data.clear();

    if(m_group.isEmpty())
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




    emit postListChanged();
    emit this->countChanged();
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
    return FMH::toMapList(getApps());
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
    emit groupChanged();
}

void AppsModel::setLimit(int limit)
{
    if (m_limit == limit)
        return;

    m_limit = limit;
    emit limitChanged();
}
