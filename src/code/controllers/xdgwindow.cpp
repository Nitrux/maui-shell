#include "xdgwindow.h"
#include <QDebug>
#include <KDesktopFile>
#include <QtWaylandCompositor/QWaylandShellSurface>
#include <QtWaylandCompositor/QWaylandXdgShell>
#include <QtWaylandCompositor/QWaylandSurface>
#include <QtWaylandCompositor/QWaylandXdgSurface>
#include <QtWaylandCompositor/QWaylandXdgToplevel>
#include <QtWaylandCompositor/QWaylandClient>
#include <QtWaylandCompositor/QWaylandCompositor>


XdgWindow::XdgWindow(QWaylandShellSurface * shellSurface, QWaylandXdgToplevel * toplevel) : AbstractWindow()
  ,m_shellSurface(shellSurface)
  ,m_toplevel(toplevel)
  ,m_xdgSurface(qobject_cast<QWaylandXdgSurface*>(m_shellSurface))
  ,m_waylandSurface( m_xdgSurface->surface())
  ,m_client(m_waylandSurface->client())
  ,m_compositor(m_waylandSurface->compositor())
{
    this->setUpToplevelConnections();

    connect(m_shellSurface, &QWaylandXdgToplevel::destroyed, [this]()
    {
        emit this->closed();
        this->deleteLater();
    });
}

XdgWindow::~XdgWindow()
{
    qDebug() << "DELETING XDG WINDOW";
    //    m_client->close();
    //    qDebug() << "WINDOW CLOSED?" << m_toplevel << m_xdgSurface;
}

QWaylandShellSurface *XdgWindow::shellSurface() const
{
    return m_shellSurface;
}

QWaylandXdgToplevel *XdgWindow::toplevel() const
{
    return m_toplevel;
}

void XdgWindow::setShellSurface(QWaylandShellSurface *shellSurface)
{
    if (m_shellSurface == shellSurface)
        return;

    m_shellSurface = shellSurface;

    m_xdgSurface = qobject_cast<QWaylandXdgSurface*>(shellSurface);
    emit this->xdgSurfaceChanged();

    m_waylandSurface = m_xdgSurface->surface();
    emit this->waylandSurfaceChanged();

    m_client = m_waylandSurface->client();
    emit this->clientChanged();

    m_compositor = m_waylandSurface->compositor();
    qDebug() << "CLIENT <<<" << m_client->processId();
    emit this->compositorChanged();

    emit shellSurfaceChanged(m_shellSurface);
}

void XdgWindow::setToplevel(QWaylandXdgToplevel *toplevel)
{
    if (m_toplevel == toplevel)
        return;

//    if(m_toplevel)
//        m_toplevel->deleteLater();

    m_toplevel = toplevel;
    qDebug() << "SETTIGN WINDOW TOPLEVEL" << toplevel << m_toplevel;

    emit toplevelChanged(m_toplevel);
}

void XdgWindow::deactivate()
{
    if(!m_toplevel)
        return;

    auto states = m_toplevel->states();
    states.removeAll(QWaylandXdgToplevel::ActivatedState);
    m_toplevel->sendConfigure(QSize(0,0), states);
}

void XdgWindow::activate()
{
    if(!m_toplevel)
        return;

    auto states = m_toplevel->states();

    if(states.contains(QWaylandXdgToplevel::ActivatedState))
    {
        return;
    }

    states << QWaylandXdgToplevel::ActivatedState;
    m_toplevel->sendConfigure(QSize(0,0), states);
}

void XdgWindow::close()
{
    m_toplevel->sendClose();
}

bool XdgWindow::maximized() const
{
    if(!m_toplevel)
        return false;

    return m_toplevel->maximized();
}

bool XdgWindow::fullscreen() const
{
    if(!m_toplevel)
        return false;

    return m_toplevel->fullscreen();

}

void XdgWindow::setUpClientConnections()
{
    if(!m_client)
        return;
}

void XdgWindow::setUpToplevelConnections()
{
    if(!m_toplevel)
        return;

    qDebug() << "Setting up tolevel connection from XDGWINDOW";

    connect(m_toplevel, &QWaylandXdgToplevel::titleChanged, this, &AbstractWindow::titleChanged);

    connect(m_toplevel, &QWaylandXdgToplevel::appIdChanged, [this]()
    {
        qDebug() << "tolevel appid changed" << this->appId();
        emit this->appIdChanged();
        emit this->appNameChanged();
        emit this->iconNameChanged();
    });

    connect(m_toplevel, &QWaylandXdgToplevel::activatedChanged, [this]()
    {
       this->setIsActive(m_toplevel->activated());
    });

    connect(m_toplevel, &QWaylandXdgToplevel::setMinimized, this, &AbstractWindow::setMinimized);
    connect(m_toplevel, &QWaylandXdgToplevel::setMaximized, this, &AbstractWindow::setMaximized);
    connect(m_toplevel, &QWaylandXdgToplevel::unsetMaximized, this, &AbstractWindow::unsetMaximized);
    connect(m_toplevel, &QWaylandXdgToplevel::unsetFullscreen, this, &AbstractWindow::unsetFullscreen);

    connect(m_toplevel, &QWaylandXdgToplevel::maximizedChanged, this, &AbstractWindow::maximizedChanged);
    connect(m_toplevel, &QWaylandXdgToplevel::fullscreenChanged, this, &AbstractWindow::fullscreenChanged);
}

QString XdgWindow::title() const
{
    if(!m_toplevel)
        return QString();

    return m_toplevel->title();
}

QString XdgWindow::appId() const
{
    if(!m_toplevel)
        return QString();

    return m_toplevel->appId();
}

QString XdgWindow::iconName() const
{
    if(!m_toplevel)
        return QString();

    KDesktopFile file(this->appId()+".desktop");
    return file.readIcon();
}

QString XdgWindow::appName() const
{
    if(!m_toplevel)
        return QString();

    KDesktopFile file(this->appId()+".desktop");
    return file.readName();
}

QWaylandXdgSurface *XdgWindow::xdgSurface() const
{
    return m_xdgSurface;
}
