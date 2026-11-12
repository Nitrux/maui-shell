#include "tasksmodel.h"

#include <QtWaylandCompositor/QWaylandSurface>

#include <QDebug>
#include <QSettings>

#include <KService>

#include "code/controllers/abstractwindow.h"
#include "code/controllers/desktoputils.h"
#include "code/controllers/task.h"
#include "code/controllers/zpaces.h"

TasksModel::TasksModel(Zpaces *parent) : QAbstractListModel(parent)
  ,m_zpaces(parent)
{
    this->setPinnedTasks();
}

TasksModel::~TasksModel()
{
    qDeleteAll(m_tasks);
}

int TasksModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return m_tasks.count();
}

QVariant TasksModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_tasks.count())
        return QVariant();

    auto task = m_tasks[index.row()];
    if (!task)
        return QVariant();

    auto window = task->window();

    switch(role)
    {
    case Roles::SurfaceIndex: return QVariant( window ? m_zpaces->indexOfWindow(window) : -1);
    case Roles::ZpaceIndex: return QVariant(window ? m_zpaces->indexOfZpace(window) : -1);
    case Roles::TaskItem: return QVariant::fromValue(task);
    default: return QVariant();
    }

    return QVariant();
}

QHash<int, QByteArray> TasksModel::roleNames() const
{
    return QHash<int, QByteArray> {{Roles::SurfaceIndex, "surfaceIndex" },
        {Roles::ZpaceIndex, "zpaceIndex" },
        {Roles::TaskItem, "task"}
    };
}

void TasksModel::addTask(AbstractWindow *window)
{
    if(!window)
        return;


    //here needs to find if the window appid belongs to an existing task.
    Task * task = nullptr;
    task = findTask(window->appId());


    if(task && task->window() == nullptr)
    {
        task->setWindow(window);
    }else
    {
        task = new Task(window->appId(), window, this);
       this->addTask(task);
    }
}

void TasksModel::addTask(const QString &id, const bool &pin)
{
    if(containsTask(id))
    {
        return;
    }

    auto task = new Task(id, nullptr, this);
    task->setIsPinned(pin);
    this->addTask(task);
}

void TasksModel::removeTask(const int &index)
{
    if(!indexIsValid(index))
        return;

    this->beginRemoveRows(QModelIndex(), index, index);
    Task *task = m_tasks.takeAt(index);
    this->endRemoveRows();

    if(task)
        task->deleteLater();

    Q_EMIT this->countChanged();
}

void TasksModel::removeTaskForWindow(AbstractWindow *window)
{
    if(!window)
        return;

    for(int i = m_tasks.size() - 1; i >= 0; --i)
    {
        Task *task = m_tasks.at(i);
        if(!task || task->window() != window)
            continue;

        if(task->isPinned())
        {
            task->setWindow(nullptr);
        }else
        {
            this->removeTask(i);
        }
    }
}

QStringList TasksModel::pinnedTasks()
{
    QStringList res;
    QSettings settings;
    settings.beginGroup("TaskBar");
    res = settings.value("Pinned").toStringList();
    settings.endGroup();
    return res;
}

int TasksModel::count() const
{
    return m_tasks.count();
}

int TasksModel::indexOf(Task *task)
{
    if(!task)
        return -1;

    return this->m_tasks.indexOf(task);
}

bool TasksModel::indexIsValid(const int &index) const
{
    if(index >= m_tasks.size() || index < 0)
        return false;

    return true;
}

void TasksModel::setPinnedTasks()
{
    for(const auto &taskId : pinnedTasks())
    {
        if(DesktopUtils::serviceForId(taskId))
        {
            this->addTask(taskId, true);
        }
    }
}

void TasksModel::addTask(Task *task)
{
    if(!task)
    {
        qWarning() << "Task is nullptr and can not be added to the TaskModel";
        return;
    }

    const auto index = m_tasks.size();

    this->beginInsertRows(QModelIndex(), index, index);
    m_tasks.append(task);
    this->endInsertRows();

    Q_EMIT this->countChanged();

    connect(task, &Task::closed, this, [this, task]()
    {
        this->removeTask(indexOf(task));
    });
}

Task *TasksModel::findTask(const QString &id)
{

    if(id.isEmpty())
        return nullptr;

    const auto service = DesktopUtils::serviceForId(id);
    if(!service)
        return nullptr;

    const auto idExec = service->exec();

    if(idExec.isEmpty())
        return nullptr;

    for(auto task : m_tasks)
    {
        if(task && task->executable() == idExec)
        {
            return task;
        }
    }

    return nullptr;
}

bool TasksModel::containsTask(const QString &id)
{
    const auto normalizedId = DesktopUtils::normalizeId(id);
    if(normalizedId.isEmpty())
        return false;

    for(auto task : m_tasks)
    {
        if(task && DesktopUtils::normalizeId(task->id()) == normalizedId)
        {
            return true;
        }
    }

    return false;
}

