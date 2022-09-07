#ifndef ABSTRACTWINDOW_H
#define ABSTRACTWINDOW_H

#include <QObject>
#include <QtWaylandCompositor/QWaylandShellSurface>

#include <QQuickItem>

class AbstractWindow : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QString appId READ appId NOTIFY appIdChanged)
    Q_PROPERTY(QString iconName READ iconName NOTIFY iconNameChanged)
    Q_PROPERTY(QString appName READ appName NOTIFY appNameChanged)

    Q_PROPERTY(bool minimized READ minimized NOTIFY minimizedChanged)
    Q_PROPERTY(bool maximized READ maximized NOTIFY maximizedChanged)
    Q_PROPERTY(bool fullscreen READ fullscreen NOTIFY fullscreenChanged)

    Q_PROPERTY(QQuickItem* chrome READ chrome WRITE setChrome NOTIFY chromeChanged)
    
    Q_PROPERTY(bool isActive READ isActive NOTIFY isActiveChanged)


private:
    bool m_minimized = false;    
    QQuickItem* m_chrome;

    bool m_isActive;

public:
    explicit AbstractWindow();

    virtual QString title() const = 0;
    virtual QString appId() const = 0;
    virtual QString iconName() const = 0;
    virtual QString appName() const = 0;
    virtual QWaylandShellSurface * shellSurface() const = 0;

    bool minimized() const;

   virtual bool maximized() const = 0;
    virtual bool fullscreen() const = 0;

    QQuickItem* chrome() const;

    bool isActive() const;
    void setIsActive(bool value);

public slots:
    virtual void close() = 0;
    virtual void minimize();
    virtual void unminimize();
    virtual void maximize();
    virtual void unmaximize();
    virtual void sendFullscreen();
    virtual void unfullscreen();

    void setChrome(QQuickItem* chrome);

signals:
    void titleChanged();
    void appIdChanged();
    void iconNameChanged();
    void appNameChanged();

    void setFullscreen();
    void unsetFullscreen();
    void setMaximized();
    void unsetMaximized();
    void setMinimized();
    void unsetMinimized();

    void minimizedChanged();
    void maximizedChanged();
    void fullscreenChanged();
    void closed();
    void chromeChanged(QQuickItem* chrome);
    void isActiveChanged(bool isActive);
};

#endif // ABSTRACTWINDOW_H
