#pragma once

#include <QObject>
#include "abstractwindow.h"

#include <QWaylandShellSurface>
#include <QWaylandXdgShell>

#include <QWaylandXdgSurface>
#include <QWaylandXdgToplevel>

class QWaylandSurface;
class QWaylandClient;

class XdgWindow : public AbstractWindow
{
    Q_OBJECT
    Q_PROPERTY(QWaylandShellSurface *shellSurface READ shellSurface WRITE setShellSurface NOTIFY shellSurfaceChanged)
    Q_PROPERTY(QWaylandXdgToplevel *toplevel READ toplevel WRITE setToplevel NOTIFY toplevelChanged)
    Q_PROPERTY(QWaylandXdgSurface *xdgSurface READ xdgSurface NOTIFY xdgSurfaceChanged)

    QWaylandShellSurface * m_shellSurface;
    QWaylandXdgToplevel *m_toplevel;
    QWaylandXdgSurface *m_xdgSurface;
    QWaylandSurface *m_waylandSurface;
    QWaylandClient *m_client;
    QWaylandCompositor *m_compositor;

public:
   explicit XdgWindow(QWaylandShellSurface *shellSurface, QWaylandXdgToplevel* toplevel);
    ~XdgWindow();
    QWaylandShellSurface * shellSurface() const override final;
    QWaylandXdgToplevel *toplevel() const;
    bool maximized() const override final;
    bool fullscreen() const override final;

public Q_SLOTS:
    void setShellSurface(QWaylandShellSurface * shellSurface);
    void setToplevel(QWaylandXdgToplevel *toplevel);

    void deactivate();
    void activate();
    void close() override final;

private:
    void setUpToplevelConnections();
    void setUpClientConnections();

Q_SIGNALS:
    void shellSurfaceChanged(QWaylandShellSurface * shellSurface);
    void toplevelChanged(QWaylandXdgToplevel *toplevel);

    void clientChanged();
    void compositorChanged();
    void waylandSurfaceChanged();
    void xdgSurfaceChanged();
    // AbstractWindow interface
public:
    QString title() const override final;
    QString appId() const override final;
    QString iconName() const override final;
    QString appName() const override final;
    QWaylandXdgSurface *xdgSurface() const;
};

