#pragma once

#include <QEvent>
#include <QObject>
#include <QUrl>
#include <QList>
#include <QScreen>


QT_FORWARD_DECLARE_CLASS(QQmlApplicationEngine)

class CaskApp : public QObject
{
    Q_OBJECT

public:
    explicit CaskApp(QObject *parent = nullptr);

    void setUrl(const QUrl &url);
    void setWindowed(bool);
    bool setScreens(const QStringList &values);

protected:
    void customEvent(QEvent *event) override;

private:
    QUrl m_url;
    QQmlApplicationEngine *m_appEngine;

    bool m_failSafe;
    bool m_started;
    bool m_windowed;
    QList<QScreen *>  m_screens;

    void verifyXdgRuntimeDir();

private Q_SLOTS:
    void startup();
    void shutdown();
    void objectCreated(QObject *object, const QUrl &);
};

class StartupEvent : public QEvent
{
public:
    StartupEvent();
};
