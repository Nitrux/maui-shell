#pragma once

#include <QObject>

#include <MauiMan4/formfactormanager.h>
#include <MauiMan4/backgroundmanager.h>
#include <MauiMan4/thememanager.h>
#include <MauiMan4/screenmanager.h>
#include <MauiMan4/inputdevicesmanager.h>

class MauiManInterface : public QObject
{
    Q_OBJECT
    Q_PROPERTY(MauiMan::BackgroundManager* background READ background CONSTANT)
    Q_PROPERTY(MauiMan::ThemeManager* theme READ theme CONSTANT)
    Q_PROPERTY(MauiMan::ScreenManager* screen READ screen CONSTANT)
    Q_PROPERTY(MauiMan::FormFactorManager* formFactor READ formFactor CONSTANT)
    Q_PROPERTY(MauiMan::InputDevicesManager* inputDevices READ inputDevices CONSTANT)

public:
    enum Mode
    {
        Desktop = 0,
        Tablet,
        Phone
    };
    Q_ENUM(Mode)

    explicit MauiManInterface(QObject *parent = nullptr);

    MauiMan::BackgroundManager* background();
    MauiMan::ThemeManager *theme();
    MauiMan::ScreenManager* screen();
    MauiMan::FormFactorManager *formFactor();
    MauiMan::InputDevicesManager *inputDevices();

public Q_SLOTS:
    void invokeManager(const QString &module);

private:
    MauiMan::BackgroundManager* m_background;
    MauiMan::ThemeManager *m_theme;
    MauiMan::ScreenManager* m_screen;
    MauiMan::FormFactorManager* m_formFactor;
    MauiMan::InputDevicesManager* m_inputDevices;
};

