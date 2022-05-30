#ifndef MAUIMANINTERFACE_H
#define MAUIMANINTERFACE_H

#include <QObject>
#include <MauiMan/backgroundmanager.h>
#include <MauiMan/thememanager.h>
#include <MauiMan/mauimanutils.h>

class MauiManInterface : public QObject
{
    Q_OBJECT
    Q_PROPERTY(MauiMan::BackgroundManager* background READ background)
    Q_PROPERTY(MauiMan::ThemeManager* theme READ theme)

public:
    explicit MauiManInterface(QObject *parent = nullptr);

    MauiMan::BackgroundManager* background() const;

    MauiMan::ThemeManager *theme() const;

public slots:
    void invokeManager(const QString &module);

private:

    MauiMan::BackgroundManager* m_background;
    MauiMan::ThemeManager *m_theme;

signals:

};

#endif // MAUIMANINTERFACE_H
