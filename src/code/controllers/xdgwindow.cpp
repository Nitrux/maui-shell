#include "xdgwindow.h"
#include "desktoputils.h"
#include <QDebug>

#include <QWaylandSurface>

#include <QWaylandClient>
#include <QWaylandCompositor>


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
        Q_EMIT this->closed();
        this->deleteLater();
    });
}

XdgWindow::~XdgWindow()
{
    //    m_client->close();
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
    if (!m_xdgSurface)
    {
        qWarning() << "XdgWindow requires an XDG shell surface";
        return;
    }

    Q_EMIT this->xdgSurfaceChanged();

    m_waylandSurface = m_xdgSurface->surface();
    Q_EMIT this->waylandSurfaceChanged();

    m_client = m_waylandSurface->client();
    Q_EMIT this->clientChanged();

    m_compositor = m_waylandSurface->compositor();
    Q_EMIT this->compositorChanged();

    Q_EMIT shellSurfaceChanged(m_shellSurface);
}

void XdgWindow::setToplevel(QWaylandXdgToplevel *toplevel)
{
    if (m_toplevel == toplevel)
        return;

    if (m_toplevel)
        disconnect(m_toplevel, nullptr, this, nullptr);

    m_toplevel = toplevel;

    setUpToplevelConnections();

    Q_EMIT toplevelChanged(m_toplevel);
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
    if(!m_toplevel)
        return;

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


    connect(m_toplevel, &QWaylandXdgToplevel::titleChanged, this, &AbstractWindow::titleChanged);

    connect(m_toplevel, &QWaylandXdgToplevel::appIdChanged, this, [this]()
    {
        Q_EMIT this->appIdChanged();
        Q_EMIT this->appNameChanged();
        Q_EMIT this->iconNameChanged();
    });

    connect(m_toplevel, &QWaylandXdgToplevel::activatedChanged, this, [this]()
    {
       this->setIsActive(m_toplevel->activated());
    });

    connect(m_toplevel, &QWaylandXdgToplevel::setMinimized, this, &AbstractWindow::setMinimized);
    connect(m_toplevel, &QWaylandXdgToplevel::setMaximized, this, &AbstractWindow::setMaximized);
    connect(m_toplevel, &QWaylandXdgToplevel::unsetMaximized, this, &AbstractWindow::unsetMaximized);
    connect(m_toplevel, &QWaylandXdgToplevel::unsetFullscreen, this, &AbstractWindow::unsetFullscreen);
    connect(m_toplevel, &QWaylandXdgToplevel::setFullscreen, this, &AbstractWindow::setFullscreen);

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

    const auto service = DesktopUtils::serviceForId(this->appId());
    return service ? service->icon() : QString();
}

QString XdgWindow::appName() const
{
    if(!m_toplevel)
        return QString();

    const auto service = DesktopUtils::serviceForId(this->appId());
    return service ? service->name() : this->appId();
}

QWaylandXdgSurface *XdgWindow::xdgSurface() const
{
    return m_xdgSurface;
}
