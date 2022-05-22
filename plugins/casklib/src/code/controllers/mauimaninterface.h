#ifndef MAUIMANINTERFACE_H
#define MAUIMANINTERFACE_H

#include <QObject>
#include <MauiMan/backgroundmanager.h>

class MauiManInterface : public QObject
{
    Q_OBJECT
    Q_PROPERTY(MauiMan::BackgroundManager* background READ background)

public:
    explicit MauiManInterface(QObject *parent = nullptr);

    MauiMan::BackgroundManager* background() const;

private:

    MauiMan::BackgroundManager* m_background;

signals:

};

#endif // MAUIMANINTERFACE_H
