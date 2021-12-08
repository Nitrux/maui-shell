#include "zpace.h"
#include "code/controllers/abstractwindow.h"
#include "code/controllers/xdgwindow.h"
#include "code/models/surfacesmodel.h"
#include "code/controllers/zpaces.h"

//the zpace needs to be clean up from the model itself
Zpace::Zpace(ZpacesModel *parent) : QObject(nullptr)
  , m_windowsModel( new SurfacesModel(this))
  , m_zpacesModelRoot(parent)
  , m_zpacesRoot(m_zpacesModelRoot->zpacesRoot())
{

    connect(m_windowsModel, &SurfacesModel::countChanged, [this]()
    {
       if(m_windowsModel->count() == 0 && m_zpacesModelRoot->count() > 1)
       {
           this->deleteLater();
       }
    });

    connect(m_zpacesRoot, &Zpaces::zmodeChanged, [this](Zpaces::ZMode)
    {
        this->setConstrains();
    });

    this->setConstrains();
}

SurfacesModel *Zpace::windows() const
{
    return m_windowsModel;
}

bool Zpace::acceptsWindow(AbstractWindow *window)
{
    Q_UNUSED(window)

    //chekc if the window min and max sizes fit in
    auto xdgWindow = qobject_cast<XdgWindow*>(window);
    if(xdgWindow)
    {

    }

    if(m_windowsModel->count() < m_limit)
    {
        return true;
    }

    return false;
}

void Zpace::addWindow(AbstractWindow *window)
{
    if(!acceptsWindow(window))
        return;

    m_windowsModel->addWindow(window);
}

AbstractWindow *Zpace::popWindow(AbstractWindow *window)
{
    return m_windowsModel->popWindow(window);
}

int Zpace::limit() const
{
    return m_limit;
}

void Zpace::setLimit(int limit)
{
    if (m_limit == limit)
        return;

    m_limit = limit;
    emit limitChanged(m_limit);
}

void Zpace::setConstrains()
{
    switch(m_zpacesRoot->zmode())
    {
    case Zpaces::ZMode::DesktopMode: return setLimit(20);
    case Zpaces::ZMode::TabletMode: return setLimit(2);
    case Zpaces::ZMode::PhoneMode:
    case Zpaces::ZMode::TVMode: return setLimit(1);
    }
}


