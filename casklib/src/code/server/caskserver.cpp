#include "caskserver.h"
#include <QDebug>
#include <CaskServer/caskpower.h>
#include <CaskServer/caskscreenshot.h>
#include <CaskServer/caskchrome.h>

CaskServer::CaskServer(QObject *parent) : QObject(parent)
  ,m_power(nullptr)
  ,m_screenshot(nullptr)
  ,m_chrome(nullptr)
{
    qRegisterMetaType<CaskPower *>("const CaskPower*"); // this is needed for QML to know of
    qRegisterMetaType<CaskScreenshot *>("const CaskScreenshot*");
    qRegisterMetaType<CaskChrome *>("const CaskChrome*");
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

CaskChrome *CaskServer::chrome()
{
    if(!m_chrome)
    {
       m_chrome = new CaskChrome(this);
    }

    return m_chrome;
}

