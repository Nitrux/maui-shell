#include "mauimaninterface.h"
#include <QDBusInterface>

#include <MauiMan/settingsstore.h>
#include <MauiMan/mauimanutils.h>


#include <MauiMan/backgroundmanager.h>
#include <MauiMan/thememanager.h>
#include <MauiMan/screenmanager.h>

MauiManInterface::MauiManInterface(QObject *parent) : QObject(parent)
  ,m_background(nullptr)
  ,m_theme(nullptr)
  ,m_screen(nullptr)
  ,m_formFactor(nullptr)
{
    qRegisterMetaType<MauiMan::BackgroundManager*>("const MauiMan::BackgroundManager*"); // this is needed for QML to know of BackgroundManager
    qRegisterMetaType<MauiMan::ThemeManager*>("const MauiMan::ThemeManager*"); // this is needed for QML to know of ThemeManager
    qRegisterMetaType<MauiMan::ScreenManager*>("const MauiMan::ScreenManager*"); // this is needed for QML to know of ScreenManager
    qRegisterMetaType<MauiMan::FormFactorManager*>("const MauiMan::FormFactorManager*"); // this is needed for QML to know of FormFactorManager
    qRegisterMetaType<MauiMan::FormFactorInfo*>("const MauiMan::FormFactorInfo*"); // this is needed for QML to know of FormFactorManager
}

MauiMan::BackgroundManager *MauiManInterface::background()
{
    if(!m_background)
    {
       m_background = new MauiMan::BackgroundManager(this);
    }

    return m_background;
}

MauiMan::ThemeManager *MauiManInterface::theme()
{
    if(!m_theme)
    {
         m_theme = new MauiMan::ThemeManager(this);
    }

    return m_theme;
}

MauiMan::ScreenManager *MauiManInterface::screen()
{
    if(!m_screen)
    {
        m_screen = new MauiMan::ScreenManager(this);
    }

    return m_screen;
}

MauiMan::FormFactorManager *MauiManInterface::formFactor()
{
    if(!m_formFactor)
    {
        m_formFactor = new MauiMan::FormFactorManager(this);
    }

    return m_formFactor;
}

void MauiManInterface::invokeManager(const QString &module)
{
    MauiManUtils::invokeManager(module);
}
