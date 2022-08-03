#ifndef MAUIMANINTERFACE_H
#define MAUIMANINTERFACE_H

#include <QObject>
#include <MauiMan/backgroundmanager.h>
#include <MauiMan/thememanager.h>
#include <MauiMan/mauimanutils.h>
#include <MauiMan/screenmanager.h>

class MauiManInterface : public QObject
{
    Q_OBJECT
    Q_PROPERTY(MauiMan::BackgroundManager* background READ background)
    Q_PROPERTY(MauiMan::ThemeManager* theme READ theme)
    Q_PROPERTY(MauiMan::ScreenManager* screen READ screen)

public:
    explicit MauiManInterface(QObject *parent = nullptr);

    MauiMan::BackgroundManager* background();

    MauiMan::ThemeManager *theme();

    MauiMan::ScreenManager* screen();

public slots:
    void invokeManager(const QString &module);

private:

    MauiMan::BackgroundManager* m_background;
    MauiMan::ThemeManager *m_theme;    
    MauiMan::ScreenManager* m_screen;

signals:

};

#endif // MAUIMANINTERFACE_H
