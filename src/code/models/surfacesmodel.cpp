#include "surfacesmodel.h"
#include "code/controllers/abstractwindow.h"
#include <QDebug>
#include <QQuickItem>

#include <QtWaylandCompositor/QWaylandXdgSurface>
#include "code/controllers/xdgwindow.h"

#include <QWaylandShellSurface>

SurfacesModel::SurfacesModel(QObject *parent) :  QAbstractListModel(parent)
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
    }, Qt::UniqueConnection);

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
        return m_windows.takeAt(index);
        this->endRemoveRows();
        emit this->countChanged();
    }

    return nullptr;
}

void SurfacesModel::removeWindow(const int &index)
{
    if(!indexIsValid(index))
        return;
qDebug() << "REMOVING WINDOW FORM MODEL";
    this->beginRemoveRows(QModelIndex(), index, index);
    auto window = this->m_windows.takeAt(index); //it is autodeleted ?
    this->endRemoveRows();
    emit this->countChanged();
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

int SurfacesModel::count() const
{
    return m_windows.count();
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
