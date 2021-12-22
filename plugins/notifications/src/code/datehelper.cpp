#include "datehelper.h"
#include <QDateTime>

DateHelper::DateHelper(QObject *parent) : QObject(parent)
{

}

QString DateHelper::friendlyTime(const QDateTime &time)
{
    QDateTime now = QDateTime::currentDateTime();
    qint64 minutes = qRound64(time.secsTo(now) / 60.0f);

    if (minutes < 1)
        return tr("Now");
    else if (minutes == 1)
        return tr("1 minute ago");
    else if (minutes < 60)
        return tr("%1 minutes ago").arg(minutes);

    qint64 hours = qRound64(minutes / 60.0f);
    if (hours == 1)
        return tr("1 hour ago");
    else if (hours < 24)
        return tr("%1 hours ago").arg(hours);

    qint64 days = qRound64(hours / 24.0f);
    if (days == 1)
        return tr("1 day ago");
    else if (days <= 10)
        return tr("%1 days ago").arg(days);

    return time.toString(Qt::DefaultLocaleShortDate);
}
