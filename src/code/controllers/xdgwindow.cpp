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

XdgWindow::XdgWindow(QObject *parent) : AbstractWindow(parent)
  ,m_shellSurface(nullptr)
  ,m_toplevel(nullptr)
  , m_xdgSurface(nullptr)
  ,m_waylandSurface(nullptr)
  ,m_client(nullptr)
  ,m_compositor(nullptr)
{
    connect(this, &XdgWindow::toplevelChanged, this, &XdgWindow::setUpToplevelConnections);
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

    if(m_toplevel)
        m_toplevel->deleteLater();

    m_toplevel = toplevel;
    qDebug() << "SETTIGN WINDOW TOPLEVEL" << toplevel << m_toplevel;

    emit toplevelChanged(m_toplevel);
}

void XdgWindow::close()
{
        m_toplevel->sendClose();
//    m_client->close();
//    this->deleteLater();
}

void XdgWindow::setUpClientConnections()
{
    if(!m_client)
        return;

//    connect(m_client, &QWaylandClient::titleChanged, this, &AbstractWindow::titleChanged);

}

void XdgWindow::setUpToplevelConnections()
{
    if(!m_toplevel)
        return;

    qDebug() << "Setting up tolevel connection from XDGWINDOW";
    connect(m_toplevel, &QWaylandXdgToplevel::destroyed, this, &XdgWindow::deleteLater);

    connect(m_toplevel, &QWaylandXdgToplevel::titleChanged, this, &AbstractWindow::titleChanged);

    connect(m_toplevel, &QWaylandXdgToplevel::appIdChanged, [this]()
    {
        qDebug() << "tolevel appid changed" << this->appId();
        emit this->appIdChanged();
        emit this->appNameChanged();
        emit this->iconNameChanged();

    });

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
