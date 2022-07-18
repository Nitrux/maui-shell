#include "caskserver.h"
#include <QDebug>

CaskServer::CaskServer(QObject *parent) : QObject(parent)
  ,m_power(nullptr)
{
        qRegisterMetaType<CaskPower *>("const CaskPower*"); // this is needed for QML to know of FMList in the search method
}

CaskPower *CaskServer::power()
{
    if(!m_power)
    {
       m_power = new CaskPower(this);
    }
    return m_power;
}

