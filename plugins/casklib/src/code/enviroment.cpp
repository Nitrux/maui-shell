#include "enviroment.h"
#include <KDesktopFile>
#include <QDebug>

Enviroment::Enviroment(QObject *parent) : QObject(parent)
{

}

void Enviroment::putenv(QString , QString )
{

}

void Enviroment::setIsMobile(const bool &value)
{
    qputenv("QT_QUICK_CONTROLS_MOBILE", value ? "1" : "0");
}

QString Enviroment::appIconName(const QString &appId)
{
    KDesktopFile file(appId+".desktop");
    qDebug() << "AppIcon name" << file.readIcon() << appId << file.fileName() ;
    return file.readIcon();
}

bool Enviroment::intersects(const QRectF first, const QRectF &second)
{
    qDebug() << "Checkign intersection" << first << second;
    return first.intersects(second);
}
