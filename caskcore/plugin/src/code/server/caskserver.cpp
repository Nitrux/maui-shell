#include "caskserver.h"
#include <QDebug>


CaskServer::CaskServer(QObject *parent) : QObject(parent)
  ,m_power(nullptr)
  ,m_screenshot(nullptr)
  ,m_chrome(nullptr)
{
//    qRegisterMetaType<CaskPower *>("const CaskPower*"); // this is needed for QML to know of
//    qRegisterMetaType<CaskScreenshot *>("const CaskScreenshot*");
//    qRegisterMetaType<CaskChrome *>("const CaskChrome*");
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


DropShadowHelper::DropShadowHelper(QObject *parent) : QObject(parent)
  ,m_chrome(new CaskChrome(this))
{
     connect(m_chrome, &CaskChrome::dropShadowChanged, [this](int radius, QString id)
     {
         qDebug() << "DROP SHADOW CHANGED FOR" << id << radius << m_id;
        if(id == m_id)
        {
         m_radius = radius;
         Q_EMIT radiusChanged(m_radius);
        }
     });

     connect(this, &DropShadowHelper::idChanged, [this](QString id)
     {
        m_radius = m_chrome->shadowFor(id);
        Q_EMIT radiusChanged(m_radius);
     });
}

QString DropShadowHelper::id() const
{
    return m_id;
}

int DropShadowHelper::radius() const
{
    return m_radius;
}

void DropShadowHelper::setId(QString id)
{
    if (m_id == id)
        return;

    m_id = id;
    Q_EMIT idChanged(m_id);
}
