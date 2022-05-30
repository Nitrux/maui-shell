#include "mauimaninterface.h"
#include <QDBusInterface>

#include <MauiMan/settingsstore.h>

MauiManInterface::MauiManInterface(QObject *parent) : QObject(parent)
  ,m_background( new MauiMan::BackgroundManager(this))
  ,m_theme( new MauiMan::ThemeManager(this))
{
    qRegisterMetaType<MauiMan::BackgroundManager*>("const MauiMan::BackgroundManager*"); // this is needed for QML to know of FMList in the search method
    qRegisterMetaType<MauiMan::ThemeManager*>("const MauiMan::ThemeManager*"); // this is needed for QML to know of FMList in the search method
}

MauiMan::BackgroundManager *MauiManInterface::background() const
{
    return m_background;
}

MauiMan::ThemeManager *MauiManInterface::theme() const
{
    return m_theme;
}

void MauiManInterface::invokeManager(const QString &module)
{
    MauiManUtils::invokeManager(module);
}
