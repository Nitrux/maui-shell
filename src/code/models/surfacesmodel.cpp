#include "surfacesmodel.h"

#include <QDebug>
#include <QQuickItem>

#include <QWaylandShellSurface>
#include <QtWaylandCompositor/QWaylandXdgSurface>

#include "code/controllers/xdgwindow.h"
#include "code/controllers/zpace.h"

SurfacesModel::SurfacesModel(Zpace *zpace) :  QAbstractListModel(zpace)
  ,m_zpace(zpace)
  ,m_activeWindow(nullptr)
{

}

SurfacesModel::SurfacesModel(QObject *parent) : QAbstractListModel(parent)
,m_zpace(nullptr)
,m_activeWindow(nullptr)
{

}

SurfacesModel::~SurfacesModel()
{

    // This model never owns the windows. Connections are context-bound to this
    // object, so they are disconnected automatically; clear the bookkeeping.
    m_windowConnections.clear();
}

void SurfacesModel::addWindow(AbstractWindow *window)
{
    if(!window || windowExists(window))
        return;

    const auto index = m_windows.size();

    QVector<QMetaObject::Connection> connections;
    connections.append(connect(window, &AbstractWindow::destroyed, this, [this, window]()
    {
        const auto windowIndex = indexOf(window);
        if(windowIndex >= 0)
            this->removeWindow(windowIndex);
    }));

    connections.append(connect(window, &AbstractWindow::activatedChanged, this, [this, window](bool value)
    {
        if(value)
        {
            this->setActiveWindow(window);
        }
    }));

    m_windowConnections.insert(window, connections);

    this->beginInsertRows(QModelIndex(), index, index);
    m_windows.append(window);
    this->endInsertRows();
    Q_EMIT this->countChanged();
}

void SurfacesModel::disconnectWindow(AbstractWindow *window)
{
    const auto connections = m_windowConnections.take(window);
    for(const auto &connection : connections)
    {
        QObject::disconnect(connection);
    }
}

AbstractWindow* SurfacesModel::popWindow(AbstractWindow *window)
{
    const auto index = indexOf(window);
    if(index < 0)
        return nullptr;

    this->beginRemoveRows(QModelIndex(), index, index);
    AbstractWindow *removed = m_windows.takeAt(index);
    this->endRemoveRows();

    disconnectWindow(removed);

    if(removed == m_activeWindow)
    {
        setActiveWindow(nullptr);
    }

    Q_EMIT this->countChanged();
    return removed;
}

void SurfacesModel::removeWindow(const int &index)
{
    if(!indexIsValid(index))
        return;

    this->beginRemoveRows(QModelIndex(), index, index);
    AbstractWindow *window = m_windows.takeAt(index);
    this->endRemoveRows();

    disconnectWindow(window);

    if(window == m_activeWindow)
    {
        setActiveWindow(nullptr);
    }

    Q_EMIT this->countChanged();
}

SurfacesModel::Windows SurfacesModel::windows() const
{
    return m_windows;
}

int SurfacesModel::indexOf(AbstractWindow *window)
{
    if(!window)
        return -1;

    return m_windows.indexOf(window);
}

bool SurfacesModel::windowExists(AbstractWindow *window)
{
    return this->indexOf(window)>= 0;
}

QQuickItem *SurfacesModel::chromeFor(AbstractWindow *window)
{
//    for(const auto &win : m_windows)
//    {
//        if(static_cast<XdgWindow*>(win)->xdgSurface()->parentSurface() == static_cast<XdgWindow*>(window)->xdgSurface()->parentSurface())
//        {
//            return win->chrome();
//        }
//    }

    return nullptr;
}

bool SurfacesModel::indexIsValid(const int &index) const
{
    if(index >= m_windows.size() || index < 0)
        return false;

    return true;
}

void SurfacesModel::setActiveWindow(AbstractWindow *window)
{    
    if(m_activeWindow == window)
    {
        return;
    }

    m_activeWindow = window;
    Q_EMIT this->activeWindowChanged(m_activeWindow);

    m_activeWindowIndex = indexOf(window);
    Q_EMIT this->activeWindowIndexChanged(m_activeWindowIndex);
}

int SurfacesModel::count() const
{
    return m_windows.count();
}

AbstractWindow *SurfacesModel::activeWindow() const
{
    return m_activeWindow;
}

int SurfacesModel::activeWindowIndex() const
{
    return m_activeWindowIndex;
}

AbstractWindow *SurfacesModel::getWindow(int index) const
{
    if(index> -1 && index < m_windows.count())
    {
        return m_windows.at(index);
    }

    return nullptr;
}

void SurfacesModel::activateNextWindow()
{
    if(m_windows.isEmpty())
        return;

    if(!m_activeWindow)
    {
        m_windows.first()->unminimize();
        return;
    }

    const auto index = indexOf(m_activeWindow);

    if(index == -1)
    {
        return;
    }

    if(index == count()-1)
    {
        m_windows.first()->unminimize();
    }else
    {
        getWindow(index+1)->unminimize();
    }
}

int SurfacesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return m_windows.count();
}

QVariant SurfacesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_windows.count())
        return QVariant();

    const auto window = m_windows.at(index.row());
    if (!window)
        return QVariant();

    switch(role)
    {
    case Roles::ShellSurface: return QVariant::fromValue(window->shellSurface());

    case Roles::Window: return QVariant::fromValue(window);

    default: return QVariant();
    }

    return QVariant();
}

QHash<int, QByteArray> SurfacesModel::roleNames() const
{
    return QHash<int, QByteArray> {{Roles::ShellSurface, "shellSurface"},{Roles::Window, "window"}};
}
