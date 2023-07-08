#include "zpaces.h"
#include "code/controllers/zpace.h"
#include <QDebug>
#include <QWaylandOutput>
#include <QScreen>
#include <QWindow>

//#include <QTouchDevice>
#include <QQuickItem>

#include <QtWaylandCompositor/QWaylandShellSurface>
#include <QtWaylandCompositor/QWaylandXdgToplevel>

#include "code/controllers/xdgwindow.h"

Zpaces::Zpaces(QObject *parent) : QObject(parent)
  ,m_zpacesModel(new ZpacesModel(this))
  ,m_tasksModel(new TasksModel(this))
  ,m_allSurfaces(new SurfacesModel(this))
  ,m_output(nullptr)
{
    connect(this, &Zpaces::outputChanged, this, &Zpaces::setGeometryConnections);
}

Zpaces::~Zpaces()
{
    qDebug() << "DELETING ZPACES";

//        m_allSurfaces->deleteLater();
//        m_tasksModel->deleteLater();
//        m_zpacesModel->deleteLater();

}

ZpacesModel *Zpaces::zpacesModel() const
{
    return m_zpacesModel;
}

TasksModel *Zpaces::tasksModel() const
{
    return m_tasksModel;
}

QWaylandOutput *Zpaces::output() const
{
    return m_output;
}

SurfacesModel *Zpaces::allSurfaces() const
{
    return m_allSurfaces;
}

int Zpaces::addWindow(AbstractWindow *window, const int &fromIndex)
{
    qDebug() << "trying to add window" << window;
    window->setParent(nullptr); //parent the window to nullptr. windows are destroyed when closed or by the SurfacesModel itself. SurfacesModel takes care of cleaning up remanining windows on destruction, same TasksModel.

    //add window to allSurfacesModel
    m_allSurfaces->addWindow(window);

    QMetaObject::Connection * const winConnection = new QMetaObject::Connection;
    *winConnection = connect(window, &AbstractWindow::appIdChanged, [this, winConnection, window]()
    {
        m_tasksModel->addTask(window);
        QObject::disconnect(*winConnection);
        delete winConnection;
    });


    for(int i = fromIndex ; i < m_zpacesModel->count(); i++)
    {
        auto zpace = m_zpacesModel->zpace(i);
        if(zpace->acceptsWindow(window))
        {
            zpace->addWindow(window);
            return i;
        }
    }

    //if we get here is beacuse none of the existing Zpaces accepted the new window, so create a new Zpace for it

    auto newZpace = m_zpacesModel->addZpace();
    newZpace->addWindow(window);
    //    dynamic_cast<XdgWindow*>(window)->toplevel()->sendFullscreen(){m_output->availableGeometry().width(), m_output->availableGeometry().height()});

    return m_zpacesModel->count()-1;
}

int Zpaces::moveWindow(AbstractWindow *window, Zpace *fromZpace, Zpace *toZpace)
{
    //if the given Zpaces are not valid return -1
    if(!fromZpace || !toZpace)
        return -1;

    //if the initial Zpace doesnt exists then return -1
    if(!m_zpacesModel->zpaceExists(fromZpace))
    {
        return -1;
    }

    //if the target Zpace doesnt exists then return the index of the initial Zpace, which by now we know it exists
    if(!m_zpacesModel->zpaceExists(toZpace))
    {
        return m_zpacesModel->indexOf(fromZpace);
    }

    if(toZpace->acceptsWindow(window))
    {
        //if the window could not be popped then quit operation
        if(fromZpace->popWindow(window))
        {
            toZpace->addWindow(window);
            return m_zpacesModel->indexOf(toZpace);
        }
    }

    return m_zpacesModel->indexOf(fromZpace);
}

void Zpaces::insertZpace(const int &index)
{
    m_zpacesModel->insertZpace(index);
}

int Zpaces::indexOfWindow(AbstractWindow *window)
{
    if(!window)
        return -1;

    for(auto zpace : m_zpacesModel->zpaces())
    {
        auto windows = zpace->windows();
        if(windows->windowExists(window))
        {
            return windows->indexOf(window);
        }
    }

    return -1;
}

QQuickItem *Zpaces::windowChrome(AbstractWindow *window)
{
    if(!window)
        return nullptr;

    for(const auto &zpace : m_zpacesModel->zpaces())
    {
        auto windows = zpace->windows();
        if(windows->windowExists(window))
        {
            return windows->chromeFor(window);
        }
    }

    return nullptr;
}

int Zpaces::indexOfZpace(AbstractWindow *window)
{
    if(!window)
        return -1;

    auto i = 0;

    for(auto zpace : m_zpacesModel->zpaces())
    {
        auto windows = zpace->windows();
        if(windows->windowExists(window))
        {
            return i;
        }

        i++;
    }

    return -1;
}

void Zpaces::setOutput(QWaylandOutput *output)
{
    if (m_output == output)
        return;

    m_output = output;
    Q_EMIT outputChanged();
}

void Zpaces::setGeometryConnections()
{
    if(!m_output)
        return;

    connect(m_output, &QWaylandOutput::geometryChanged, this, &Zpaces::setZpacesMode);
}

void Zpaces::setZpacesMode()
{
    if(!m_output)
        return;

    qDebug() << "OUTPUT GEOMETRY CHANGED";
    auto geometry = m_output->geometry();

    //    auto position = m_output->position();
    //    auto physicalSize = m_output->physicalSize();

    //    auto screen = m_output->window()->screen();
    //    bool devicePixelRatio = screen->devicePixelRatio();
    qDebug() << "CHECKING FOR TOUCH DEVICES";
    //for(const auto &device : QTouchDevice::devices())
    //{
    //    qDebug() << "DEVICE" << device->capabilities() <<device->maximumTouchPoints() << device->name() << device->type();
    //}

    if(geometry.width() > 1500)
    {
        m_zmode = ZMode::DesktopMode;
    }else if(geometry.width() > 500)
    {
        m_zmode = ZMode::TabletMode;
    }else
    {
        m_zmode = ZMode::PhoneMode;
    }

    Q_EMIT this->zmodeChanged(m_zmode);
}

Zpaces::ZMode Zpaces::zmode() const
{
    return m_zmode;
}

XdgWindow *Zpaces::createXdgWindow(QWaylandShellSurface *shellSurface, QWaylandXdgToplevel * toplevel)
{
    return new XdgWindow(shellSurface, toplevel);
}

void Zpaces::clearAllSurfaces()
{
   for(auto window : m_allSurfaces->windows())
   {
       window->close();
   }
}
