#include "caskserver.h"
#include <QDebug>
#include <CaskServer/caskpower.h>
#include <CaskServer/caskscreenshot.h>

CaskServer::CaskServer(QObject *parent) : QObject(parent)
  ,m_power(nullptr)
  ,m_screenshot(nullptr)
{
    qRegisterMetaType<CaskPower *>("const CaskPower*"); // this is needed for QML to know of
    qRegisterMetaType<CaskScreenshot *>("const CaskScreenshot*");
}

CaskPower *CaskServer::power()
{
    if(!m_power)
    {
       m_power = new CaskPower(this);
    }
    return m_power;
}

CaskScreenshot *CaskServer::screenshot()
{
    if(!m_screenshot)
    {
       m_screenshot = new CaskScreenshot(this);
    }

    return m_screenshot;
}

