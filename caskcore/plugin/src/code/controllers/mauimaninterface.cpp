#include "mauimaninterface.h"
#include <QDBusInterface>

#include <MauiMan4/settingsstore.h>
#include <MauiMan4/mauimanutils.h>

MauiManInterface::MauiManInterface(QObject *parent) : QObject(parent)
  ,m_background(nullptr)
  ,m_theme(nullptr)
  ,m_screen(nullptr)
  ,m_formFactor(nullptr)
  ,m_inputDevices(nullptr)
{
//    qRegisterMetaType<MauiMan::BackgroundManager*>("const MauiMan::BackgroundManager*"); // this is needed for QML to know of BackgroundManager
//    qRegisterMetaType<MauiMan::ThemeManager*>("const MauiMan::ThemeManager*"); // this is needed for QML to know of ThemeManager
//    qRegisterMetaType<MauiMan::ScreenManager*>("const MauiMan::ScreenManager*"); // this is needed for QML to know of ScreenManager
//    qRegisterMetaType<MauiMan::FormFactorManager*>("const MauiMan::FormFactorManager*"); // this is needed for QML to know of FormFactorManager
//    qRegisterMetaType<MauiMan::FormFactorInfo*>("const MauiMan::FormFactorInfo*"); // this is needed for QML to know of FormFactorInfo
//    qRegisterMetaType<MauiMan::InputDevicesManager*>("const MauiMan::InputDevicesManager*"); // this is needed for QML to know of InputDevices
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

MauiMan::InputDevicesManager *MauiManInterface::inputDevices()
{
    if(!m_inputDevices)
    {
        m_inputDevices = new MauiMan::InputDevicesManager(this);
        connect(m_inputDevices, &MauiMan::InputDevicesManager::keyboardLayoutChanged, [this](const QString &layout)
        {
            qputenv("XKB_DEFAULT_LAYOUT", layout.toUtf8());
        });
    }

    return m_inputDevices;
}

void MauiManInterface::invokeManager(const QString &module)
{
    MauiManUtils::invokeManager(module);
}
