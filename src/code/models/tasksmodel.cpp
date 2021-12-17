#include "tasksmodel.h"
#include "code/controllers/zpaces.h"
#include <QtWaylandCompositor/QWaylandSurface>
#include <QDebug>
#include <KService>
#include <KDesktopFile>
#include <QSettings>

#include "code/controllers/abstractwindow.h"
#include "code/controllers/task.h"

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
    if (!index.isValid())
        return QVariant();

    auto task = m_tasks[index.row()];
    auto window = task->window();

    qDebug() << "Askign for taksbar mdoel data" << role;
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
    qDebug() << "trying to insert a task" << window << window->appId();

    //here needs to find if the window appid belongs to an existing task.
    Task * task = nullptr;
    task = findTask(window->appId());

    qDebug() << "Trying to deal with new window <<" << window->appId() << (task == nullptr);

    if(task && task->window() == nullptr)
    {
        qDebug() << "Setting window to existing task" << task << (task->window() == nullptr);
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
    m_tasks.remove(index);
    this->endRemoveRows();
    emit this->countChanged();
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
        this->addTask(taskId, true);
    }
}

void TasksModel::addTask(Task *task)
{
    if(!task)
    {
        qWarning() << "Task is nullptr and can not be added to the TaskModel";
    }

    const auto index = m_tasks.size();

    this->beginInsertRows(QModelIndex(), index, index);
    m_tasks.append(task);
    this->endInsertRows();

    emit this->countChanged();

    connect(task, &Task::closed, this, [this, task]()
    {
        this->removeTask(indexOf(task));
    }, Qt::UniqueConnection);
}

Task *TasksModel::findTask(const QString &id)
{
    qDebug() << "task Service exec" << id;

    if(!id.isEmpty())
    {
        QString suffix;
        if(!id.endsWith(".desktop"))
        {
            suffix = ".desktop";
        }
        KDesktopFile file(id+suffix);
        KService service(&file);
        auto idExec = service.exec();

        qDebug() << "task Service exec" << idExec;

        if(idExec.isEmpty())
        {
            return nullptr;
        }

        for(auto task : m_tasks)
        {
            if(task->executable() == idExec)
            {
                return task;
            }
        }
    }

    return nullptr;
}

bool TasksModel::containsTask(const QString &id)
{
    for(auto task : m_tasks)
    {
        if(task->id() == id)
        {
            return true;
        }
    }

    return false;
}

