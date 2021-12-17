#include "task.h"
#include "code/controllers/abstractwindow.h"
#include "code/models/tasksmodel.h"
#include <KDesktopFile>
#include <KService>

Task::Task(const QString &id, AbstractWindow *window, TasksModel *modelRoot) : QObject(nullptr) //tasksmodel destroys it
  ,m_id(id)
  ,m_window(window)
  ,m_modelRoot(modelRoot)
{
    connect(this, &Task::windowChanged, this, &Task::setWindowConnections);
    connect(this, &Task::idChanged, this, &Task::setData);

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

QString Task::executable() const
{
    return m_executable;
}

bool Task::isPinned() const
{
    return m_isPinned;
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

//    m_window->disconnect();

    connect(m_window, &AbstractWindow::destroyed, this, [this]()
    {
        if(m_isPinned)
        {
            m_window = nullptr;
            emit windowChanged();
        }else
        {
            this->deleteLater();
        }

    }, Qt::UniqueConnection);


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
