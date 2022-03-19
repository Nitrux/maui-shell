#include "task.h"
#include "code/controllers/abstractwindow.h"
#include "code/models/tasksmodel.h"

#include <KDesktopFile>
#include <KService>
#include <QSettings>

Task::Task(const QString &id, AbstractWindow *window, TasksModel *modelRoot) : QObject(nullptr)
  ,m_id(id)
  ,m_window(window)
  ,m_modelRoot(modelRoot)
{
    connect(this, &Task::windowChanged, this, &Task::setWindowConnections);
    connect(this, &Task::idChanged, this, &Task::setData);

    connect(this, &Task::isPinnedChanged, [this](bool pinned)
    {
       if(pinned)
       {
           this->pinTask();
       }else
       {
           this->unPinTask();
       }
    });

    this->setWindowConnections();
    this->setData();
}

AbstractWindow *Task::window()
{
    return m_window;
}

void Task::setWindow(AbstractWindow *window)
{
    if(window == m_window)
    {
        return;
    }

    m_window = window;
    emit windowChanged();
}

void Task::setId(const QString &id)
{
    if(m_id == id)
    {
        return;
    }

    m_id = id;
    emit idChanged();
}

QString Task::name() const
{
    return QString();
}

QString Task::iconName() const
{
    return m_iconName;
}

QString Task::fileName() const
{
    return m_fileName;
}

QString Task::id() const
{
    return m_id;
}

void Task::setName(const QString &name)
{
    if(name == m_name)
    {
        return;
    }

    m_name = name;
    emit nameChanged(m_name);
}

void Task::setIconName(const QString &name)
{
    if(name == m_iconName)
    {
        return;
    }

    m_iconName = name;
    emit this->iconNameChanged(m_iconName);
}

void Task::setPath(const QString &path)
{
    if(path == m_path)
    {
        return;
    }

    m_path = path;
    emit this->pathChanged(m_path);
}

QString Task::executable() const
{
    return m_executable;
}

bool Task::isPinned() const
{
    return m_isPinned;
}

QString Task::path() const
{
    return m_path;
}

void Task::setIsPinned(bool isPinned)
{
    if (m_isPinned == isPinned)
        return;

    m_isPinned = isPinned;
    emit isPinnedChanged(m_isPinned);
}

void Task::setWindowConnections()
{
    if(!m_window)
        return;

    connect(m_window, &AbstractWindow::closed, this, [this]()
    {
        if(m_isPinned)
        {
            m_window = nullptr;
            emit windowChanged();
        }else
        {
            emit this->closed();
        }

    }, Qt::QueuedConnection);


    connect(m_window, &AbstractWindow::appIdChanged, [this]()
    {
        this->setId(m_window->appId());
    });
}

void Task::setData()
{
    if(m_id.isEmpty())
    {
        return;
    }
    QString suffix;
    if(!m_id.endsWith(".desktop"))
    {
        suffix = ".desktop";
    }
    KDesktopFile file(m_id+suffix);
    this->setIconName(file.readIcon());
    this->setName(file.readName());
    this->setFileName(file.fileName());
    KService service(&file);
    this->setPath(service.entryPath());
    this->setExecutable(service.exec());
}

void Task::setFileName(const QString &name)
{
    if(name == m_fileName)
    {
        return;
    }

    m_fileName= name;
    emit this->fileNameChanged();
}

void Task::setExecutable(const QString &exec)
{
    if(exec == m_executable)
    {
        return;
    }

    m_executable = exec;
    emit this->executableChanged(m_executable);
}

void Task::pinTask()
{
    auto tasks = m_modelRoot->pinnedTasks();
    if(tasks.contains(m_id))
    {
        return;
    }

    tasks << m_id;

    QSettings settings;
    settings.beginGroup("TaskBar");
    settings.setValue("Pinned", tasks);
    settings.endGroup();
}

void Task::unPinTask()
{
    auto tasks = m_modelRoot->pinnedTasks();
    if(!tasks.contains(m_id))
    {
        return;
    }

    tasks.removeOne(m_id);

    QSettings settings;
    settings.beginGroup("TaskBar");
    settings.setValue("Pinned", tasks);
    settings.endGroup();

    //now chekc if it has a window, if it has then leave it there, otherwise remove it
    if(m_window == nullptr)
    {
        emit this->closed();
    }
}
