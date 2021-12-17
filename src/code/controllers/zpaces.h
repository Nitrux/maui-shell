#ifndef ZPACES_H
#define ZPACES_H

#include <QObject>
#include "code/models/zpacesmodel.h"
#include "code/models/tasksmodel.h"

class AbstractWindow;
class Zpace;
class QWaylandOutput;
class QWaylandXdgToplevel;
class XdgWindow;
class QWaylandShellSurface;
class SurfacesModel;
class Zpaces : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ZpacesModel* zpacesModel READ zpacesModel FINAL CONSTANT)
    Q_PROPERTY(TasksModel *tasksModel READ tasksModel FINAL CONSTANT)
    Q_PROPERTY(ZMode zmode READ zmode NOTIFY zmodeChanged)
    Q_PROPERTY(QWaylandOutput *output READ output WRITE setOutput NOTIFY outputChanged)
    Q_PROPERTY(SurfacesModel *allSurfaces READ allSurfaces FINAL CONSTANT)

public:
    enum ZMode
    {
        TVMode,
        DesktopMode,
        TabletMode,
        PhoneMode
    }; Q_ENUM(ZMode)
    explicit Zpaces(QObject *parent = nullptr);
~Zpaces();
    /**
     * @brief zpacesModel
     * Model of all the Zpaces AKA workspaces
     * @return
     */
    ZpacesModel* zpacesModel() const;

    TasksModel * tasksModel() const;

    QWaylandOutput * output() const;

    SurfacesModel * allSurfaces() const;

public slots:
    /**
     * @brief addSurface
     * Adds a window to the right Zpace. To add a new window, a fitting Zpace has to be found first, that's an existing one or a new one.
     * @param window
     * Surface to be added
     * @param fromIndex
     * Index of the current Zpace, and start checkign avaliabilty from it forwards
     * @return
     * Returns the index of the Zpace where the window was added to
     */
    int addWindow(AbstractWindow *window, const int &fromIndex = 0);

    /**
     * @brief moveSurface
     * Moves a window form a given Zpace to a new Zpace
     * @param window
     * Surface to be moved
     * @param fromZpace
     * Intial Zpace where the window was at
     * @param toZpace
     * Target Zpace to move the window to
     * @return
     * If the window was successfuly moved then return the index of the new ZPace on the ZpacesModel, otherwise returns the index of the fromZpace.
     */
    int moveWindow(AbstractWindow *window, Zpace* fromZpace, Zpace *toZpace);

    /**
     * @brief insertZpace
     * Helper method to insert a new Zpace at a given index
     * @param index
     * Where to insert the new Zpace
     */
    void insertZpace(const int &index);

    /**
     * @brief indexOfSurface
     * Index of a window within its current Zpace
     * @param window
     * @return
     */
    int indexOfWindow(AbstractWindow *window);

    /**
     * @brief indexOfZpace
     * Index of a window Zpace.
     * @param window
     * @return
     */
    int indexOfZpace(AbstractWindow* window);

    /**
     * @brief setOutput
     * The output which layouts this Zpaces. Usually a single screen
     * @param output
     */
    void setOutput(QWaylandOutput * output);

    /**
     * @brief zmode
     * @return
     */
    ZMode zmode() const;

    XdgWindow *createXdgWindow(QWaylandShellSurface *, QWaylandXdgToplevel*);

    void clearAllSurfaces();

private:
    ZpacesModel *m_zpacesModel;    
    TasksModel * m_tasksModel;
    SurfacesModel * m_allSurfaces;

    QWaylandOutput * m_output;

    ZMode m_zmode = ZMode::DesktopMode;

    void setGeometryConnections();
    void setZpacesMode();


signals:

    void outputChanged();
    void zmodeChanged(ZMode mode);
};

#endif // ZPACES_H
