#ifndef POWERSERVER_H
#define POWERSERVER_H

#include <QObject>
#include "casklib_export.h"

class CASKLIB_EXPORT PowerServer : public QObject
{
    Q_OBJECT
public:
    explicit PowerServer(QObject *parent = nullptr);

    Q_INVOKABLE void restart();
    Q_INVOKABLE void logout();
    Q_INVOKABLE void shutdown();
    Q_INVOKABLE void sleep();

signals:
    void restartRequested();
    void logoutRequested();
    void shutdownRequested();
    void sleepRequested();

    void quit();
};

#endif // POWERSERVER_H
