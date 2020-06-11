#include "enviroment.h"

Enviroment::Enviroment(QObject *parent) : QObject(parent)
{

}

void Enviroment::putenv(QString key, QString value)
{

}

void Enviroment::setIsMobile(const bool &value)
{
        qputenv("QT_QUICK_CONTROLS_MOBILE", value ? "1" : "0");
}
