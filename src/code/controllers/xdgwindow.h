#ifndef XDGWINDOW_H
#define XDGWINDOW_H

#include <QObject>
#include "abstractwindow.h"

class QWaylandXdgToplevel;
class QWaylandXdgSurface;
class QWaylandSurface;
class QWaylandClient;
class XdgWindow : public AbstractWindow
{
    Q_OBJECT
    Q_PROPERTY(QWaylandShellSurface *shellSurface READ shellSurface WRITE setShellSurface NOTIFY shellSurfaceChanged)
    Q_PROPERTY(QWaylandXdgToplevel *toplevel READ toplevel WRITE setToplevel NOTIFY toplevelChanged)

    QWaylandShellSurface * m_shellSurface;
    QWaylandXdgToplevel *m_toplevel;
    QWaylandXdgSurface *m_xdgSurface;
    QWaylandSurface *m_waylandSurface;
    QWaylandClient *m_client;
    QWaylandCompositor *m_compositor;

public:
    explicit XdgWindow(QObject *parent = nullptr);
    ~XdgWindow();
    QWaylandShellSurface * shellSurface() const override final;
    QWaylandXdgToplevel *toplevel() const;

public slots:
    void setShellSurface(QWaylandShellSurface * shellSurface);
    void setToplevel(QWaylandXdgToplevel *toplevel);
    void close() override final;

private:
    void setUpToplevelConnections();
    void setUpClientConnections();

signals:
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
};

#endif // XDGWINDOW_H
