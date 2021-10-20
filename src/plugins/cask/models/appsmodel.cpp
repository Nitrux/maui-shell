#include "appsmodel.h"

#include <KConfig>
#include <KConfigGroup>
#include <KLocalizedString>
#include <KMimeTypeTrader>
#include <KRun>
#include <KService>
#include <KServiceGroup>
#include <KToolInvocation>
#include <QColor>
#include <QDebug>
#include <QFileInfo>
#include <QModelIndex>
#include <QVariantList>


#include <MauiKit/FileBrowsing/fmstatic.h>

AppsModel::AppsModel(QObject *parent) : MauiList(parent)
{
    this->setList();
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

        if (p->isType(KST_KServiceGroup)) {
            KServiceGroup::Ptr s(static_cast<KServiceGroup *>(p.data()));

            if (!s->noDisplay() && s->childCount() > 0) {
                res << FMH::MODEL {{FMH::MODEL_KEY::COMMENT, s->comment()}, {FMH::MODEL_KEY::ICON, s->icon()}, {FMH::MODEL_KEY::LABEL, s->name()}, {FMH::MODEL_KEY::PATH, FMStatic::PATHTYPE_URI[FMStatic::PATHTYPE_KEY::APPS_PATH] + s->entryPath()}};

            }
        }
    }
    return res;
}

static FMH::MODEL_LIST getApps(const QString &groupStr)
{
    const auto grp = QString(groupStr).replace("/", "") + "/";
    qDebug() << "APP GROUDP" << groupStr << grp;

    if (grp.isEmpty())
        return getApps();

    FMH::MODEL_LIST res;
    //    const KServiceGroup::Ptr group(static_cast<KServiceGroup*>(groupStr));
    auto group = new KServiceGroup(grp);
    KServiceGroup::List list = group->entries(true /* sorted */, true /* excludeNoDisplay */, false /* allowSeparators */, true /* sortByGenericName */);

    for (KServiceGroup::List::ConstIterator it = list.constBegin(); it != list.constEnd(); it++) {
        const KSycocaEntry::Ptr p = (*it);

        if (p->isType(KST_KService)) {
            const KService::Ptr s(static_cast<KService *>(p.data()));

            if (s->noDisplay())
                continue;

            res << FMH::MODEL {{FMH::MODEL_KEY::COMMENT, s->comment()}, {FMH::MODEL_KEY::ICON, s->icon()}, {FMH::MODEL_KEY::EXECUTABLE, "true"}, {FMH::MODEL_KEY::LABEL, s->name()}, {FMH::MODEL_KEY::PATH, s->entryPath()}};

        } else if (p->isType(KST_KServiceSeparator)) {
            qDebug() << "separator wtf";

        } else if (p->isType(KST_KServiceGroup)) {
            const KServiceGroup::Ptr s(static_cast<KServiceGroup *>(p.data()));

            if (s->childCount() == 0)
                continue;
            res << FMH::MODEL {{FMH::MODEL_KEY::ICON, s->icon()}, {FMH::MODEL_KEY::EXECUTABLE, "true"}, {FMH::MODEL_KEY::LABEL, s->name()}, {FMH::MODEL_KEY::PATH, FMStatic::PATHTYPE_URI[FMStatic::PATHTYPE_KEY::APPS_PATH] + s->entryPath()}};
        }
    }

    return res;
}

void AppsModel::setList()
{
    emit preListChanged();
    m_data.clear();

    for(const auto item : getApps())
    {
        m_data << getApps(item[FMH::MODEL_KEY::LABEL]);
    }

    emit postListChanged();
}
