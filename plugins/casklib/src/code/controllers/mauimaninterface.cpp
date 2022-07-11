#include "mauimaninterface.h"
#include <QDBusInterface>

#include <MauiMan/settingsstore.h>

MauiManInterface::MauiManInterface(QObject *parent) : QObject(parent)
  ,m_background(nullptr)
  ,m_theme(nullptr)
  ,m_screen(nullptr)
{
    qRegisterMetaType<MauiMan::BackgroundManager*>("const MauiMan::BackgroundManager*"); // this is needed for QML to know of FMList in the search method
    qRegisterMetaType<MauiMan::ThemeManager*>("const MauiMan::ThemeManager*"); // this is needed for QML to know of FMList in the search method
    qRegisterMetaType<MauiMan::ThemeManager*>("const MauiMan::ThemeManager*"); // this is needed for QML to know of FMList in the search method

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

void MauiManInterface::invokeManager(const QString &module)
{
    MauiManUtils::invokeManager(module);
}
