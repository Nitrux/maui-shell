#pragma once

#include <QObject>
#include <MauiMan/formfactormanager.h>

namespace MauiMan
{
class BackgroundManager;
class ThemeManager;
class ScreenManager;
class InputDevicesManager;
}

class MauiManInterface : public QObject
{
    Q_OBJECT
    Q_PROPERTY(MauiMan::BackgroundManager* background READ background)
    Q_PROPERTY(MauiMan::ThemeManager* theme READ theme)
    Q_PROPERTY(MauiMan::ScreenManager* screen READ screen)
    Q_PROPERTY(MauiMan::FormFactorManager* formFactor READ formFactor)
    Q_PROPERTY(MauiMan::InputDevicesManager* inputDevices READ inputDevices)

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

Q_SIGNALS:

};

