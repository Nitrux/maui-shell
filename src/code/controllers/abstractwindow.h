#ifndef ABSTRACTWINDOW_H
#define ABSTRACTWINDOW_H

#include <QObject>
#include <QtWaylandCompositor/QWaylandShellSurface>

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

private:
    bool m_minimized = false;

public:
    explicit AbstractWindow(QObject *parent = nullptr);

    virtual QString title() const = 0;
    virtual QString appId() const = 0;
    virtual QString iconName() const = 0;
    virtual QString appName() const = 0;
    virtual QWaylandShellSurface * shellSurface() const = 0;

    bool minimized() const;

   virtual bool maximized() const = 0;
    virtual bool fullscreen() const = 0;

public slots:
    virtual void close() = 0;
    virtual void minimize();
    virtual void unminimize();
    virtual void maximize();
    virtual void unmaximize();
    virtual void sendFullscreen();
    virtual void unfullscreen();

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
};

#endif // ABSTRACTWINDOW_H
