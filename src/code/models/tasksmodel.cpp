#include "tasksmodel.h"
#include "code/controllers/zpaces.h"
#include <QtWaylandCompositor/QWaylandSurface>
#include <QDebug>

#include "code/controllers/abstractwindow.h"

TasksModel::TasksModel(Zpaces *parent) : QAbstractListModel(parent)
  ,m_zpaces(parent)
{

}

int TasksModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return m_windows.count();
}

QVariant TasksModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    auto window = m_windows[index.row()];

    qDebug() << "Askign for taksbar mdoel data" << role;
    switch(role)
    {
    case Roles::SurfaceIndex: return QVariant(m_zpaces->indexOfWindow(window));
    case Roles::ZpaceIndex: return QVariant(m_zpaces->indexOfZpace(window));
    case Roles::Window: return QVariant::fromValue(window);
    default: return QVariant();
    }

    return QVariant();
}

QHash<int, QByteArray> TasksModel::roleNames() const
{
    return QHash<int, QByteArray> {{Roles::SurfaceIndex, "surfaceIndex" },
        {Roles::ZpaceIndex, "zpaceIndex" },
        {Roles::Window, "window" },
    };
}

void TasksModel::addTask(AbstractWindow *window)
{
    qDebug() << "trying to insert a task" << window;
    connect(window, &AbstractWindow::destroyed, this, [this, window]()
    {
        this->removeTask(indexOf(window));
    }, Qt::UniqueConnection);

    const auto index = m_windows.size();

    this->beginInsertRows(QModelIndex(), index, index);
    m_windows.append(window);
    this->endInsertRows();

    emit this->countChanged();
}

void TasksModel::removeTask(const int &index)
{
    if(!indexIsValid(index))
        return;

    this->beginRemoveRows(QModelIndex(), index, index);
    m_windows.remove(index);
    this->endRemoveRows();
    emit this->countChanged();
}

int TasksModel::count() const
{
    return m_windows.count();
}

int TasksModel::indexOf(AbstractWindow *window)
{
    if(!window)
        return -1;

    return this->m_windows.indexOf(window);
}

bool TasksModel::indexIsValid(const int &index) const
{
    if(index >= m_windows.size() || index < 0)
        return false;

    return true;
}
