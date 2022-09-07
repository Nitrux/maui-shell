#ifndef SURFACESMODEL_H
#define SURFACESMODEL_H

#include <QAbstractListModel>
#include <QObject>

class QQuickItem;
class AbstractWindow;
class SurfacesModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(AbstractWindow* activeWindow READ activeWindow NOTIFY activeWindowChanged FINAL)
    Q_PROPERTY(int activeWindowIndex READ activeWindowIndex NOTIFY activeWindowIndexChanged FINAL)

public:
    typedef QVector<AbstractWindow*> Windows;

    enum Roles
    {
        ShellSurface,
        Window
    };

    explicit SurfacesModel(QObject *parent = nullptr);
    ~SurfacesModel();

    int rowCount(const QModelIndex &parent) const override final;
    QVariant data(const QModelIndex &index, int role) const override final;
    QHash<int, QByteArray> roleNames() const override final;

    /**
     * @brief addWindow
     * Add a window to the model
     * @param window
     */
    void addWindow(AbstractWindow * window);

    /**
     * @brief popWindow
     * Remove a window from the model withouth deleting it
     * @param window
     */
    AbstractWindow* popWindow(AbstractWindow * window);


    /**
     * @brief windows
     * A list of all the surfaces in the model
     * @return
     */
    Windows windows() const;

    /**
     * @brief windowExists
     * Checks if a given window exists in the model
     * @param window
     * @return
     */
    bool windowExists(AbstractWindow *window);

    /**
     * @brief chromeFor
     * Find a Chrome for a window base on surface parent relationship
     * @param window
     * @return
     */
    QQuickItem *chromeFor(AbstractWindow *window);

    /**
     * @brief count
     * Total count of surfaces in the model
     * @return
     */
    int count() const;
    
    AbstractWindow* activeWindow() const;

    int activeWindowIndex() const;

public slots:
    /**
     * @brief removeWindow
     * Removes a window form the model and deletes it
     * @param index
     */
    void removeWindow(const int &index);


    /**
     * @brief indexOf
     * Index of a window in the model
     * @param window
     * @return
     */
    int indexOf(AbstractWindow *window);
    AbstractWindow* getWindow(int index) const;

    void activateNextWindow();

private:
    Windows m_windows;
    bool indexIsValid(const int &index) const;
    
    AbstractWindow* m_activeWindow;
    void setActiveWindow(AbstractWindow * window);
    
    int m_activeWindowIndex = -1;

signals:
    void countChanged();
    void activeWindowChanged(AbstractWindow* activeWindow);
    void activeWindowIndexChanged(int activeWindowIndex);
};

#endif // SURFACESMODEL_H
