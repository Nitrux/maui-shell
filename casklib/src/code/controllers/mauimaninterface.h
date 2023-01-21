#pragma once

#include <QObject>
#include <MauiMan/formfactormanager.h>

namespace MauiMan
{
    class BackgroundManager;
    class ThemeManager;
    class ScreenManager;
    class FormFactorManager;
}
class MauiManInterface : public QObject
{
    Q_OBJECT
    Q_PROPERTY(MauiMan::BackgroundManager* background READ background)
    Q_PROPERTY(MauiMan::ThemeManager* theme READ theme)
    Q_PROPERTY(MauiMan::ScreenManager* screen READ screen)
    Q_PROPERTY(MauiMan::FormFactorManager* formFactor READ formFactor)

public:
    Q_ENUM(MauiMan::FormFactorManager::Mode)

    explicit MauiManInterface(QObject *parent = nullptr);

    MauiMan::BackgroundManager* background();
    MauiMan::ThemeManager *theme();
    MauiMan::ScreenManager* screen();
    MauiMan::FormFactorManager *formFactor();

public slots:
    void invokeManager(const QString &module);

private:

    MauiMan::BackgroundManager* m_background;
    MauiMan::ThemeManager *m_theme;    
    MauiMan::ScreenManager* m_screen;
    MauiMan::FormFactorManager* m_formFactor;

signals:

};

