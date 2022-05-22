#include "mauimaninterface.h"
#include <QDBusInterface>

#include <MauiMan/settingsstore.h>

MauiManInterface::MauiManInterface(QObject *parent) : QObject(parent)
  ,m_background( new MauiMan::BackgroundManager(this))
{
    qRegisterMetaType<MauiMan::BackgroundManager*>("const MauiMan::BackgroundManager*"); // this is needed for QML to know of FMList in the search method
}

MauiMan::BackgroundManager *MauiManInterface::background() const
{
    return m_background;
}
