#include "surfacesmodel.h"
#include "code/controllers/abstractwindow.h"
#include <QDebug>
#include <QQuickItem>

#include <QtWaylandCompositor/QWaylandXdgSurface>
#include "code/controllers/xdgwindow.h"

#include <QWaylandShellSurface>

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
    // make sure to delete all the surfaces
    qDebug() << "DELETING SURFACES MODEL" << m_windows.size();

    qDeleteAll(m_windows);
}

void SurfacesModel::addWindow(AbstractWindow *window)
{
    const auto index = m_windows.size();

    connect(window, &AbstractWindow::destroyed, this, [this, window]()
    {
        this->removeWindow(indexOf(window));
    });

    connect(window, &AbstractWindow::activatedChanged, this, [this, window](bool value)
    {
        if(value)
        {
            this->setActiveWindow(window);
        }
    });

    this->beginInsertRows(QModelIndex(), index, index);
    m_windows.append(window);
    this->endInsertRows();
    emit this->countChanged();
}

AbstractWindow* SurfacesModel::popWindow(AbstractWindow *window)
{
    if(windowExists(window))
    {
        auto index = indexOf(window);
        this->beginRemoveRows(QModelIndex(), index, index);
        auto window =  m_windows.takeAt(index);
        this->endRemoveRows();
        emit this->countChanged();
        window->disconnect();

        if(window == m_activeWindow)
        {
            setActiveWindow(nullptr);
        }
        return window;
    }

    return nullptr;
}

void SurfacesModel::removeWindow(const int &index)
{
    if(!indexIsValid(index))
        return;

qDebug() << "REMOVING WINDOW FORM MODEL";

//if(count() > 1)
//{
//    if(auto control = m_zpace->control())
//    {
//       auto chrome = control->childItems().at(control->childItems().count()-2);
//       if(chrome)
//       auto window = chrome->property("window").value<XdgWindow*>();
//       window->activate();
//    }


//}else
//{
//    setActiveWindow(nullptr);
//}
    this->beginRemoveRows(QModelIndex(), index, index);
    auto window = this->m_windows.takeAt(index); //it is autodeleted ?
    this->endRemoveRows();
    Q_EMIT this->countChanged();
//    window->disconnect();
    window->deleteLater();
    qDebug() << "REMOVING WINDOW FORM MODEL" << this->count();

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
    emit this->activeWindowChanged(m_activeWindow);

    m_activeWindowIndex = indexOf(window);
    emit this->activeWindowIndexChanged(m_activeWindowIndex);
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
    if (!index.isValid())
        return QVariant();

    qDebug() << "REQUESTING FORF A WINDOW <<" << role;
    switch(role)
    {
    case Roles::ShellSurface: return QVariant::fromValue(m_windows[index.row()]->shellSurface());

    case Roles::Window: return QVariant::fromValue(m_windows[index.row()]);

    default: return QVariant();
    }

    return QVariant();
}

QHash<int, QByteArray> SurfacesModel::roleNames() const
{
    return QHash<int, QByteArray> {{Roles::ShellSurface, "shellSurface"},{Roles::Window, "window"}};
}
