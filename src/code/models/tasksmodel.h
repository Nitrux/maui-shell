#ifndef TASKSMODEL_H
#define TASKSMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <QtWaylandCompositor/QWaylandShellSurface>

class Zpaces;
class AbstractWindow;
class TasksModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged FINAL)

public:
    enum Roles
    {
        ZpaceIndex,
        SurfaceIndex,       
        Window
    };
    explicit TasksModel(Zpaces *parent = nullptr);

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const override final;
    QVariant data(const QModelIndex &index, int role) const override final;
    QHash<int, QByteArray> roleNames() const override final;

    /**
     * @brief count
     * @return
     */
    int count() const;

    /**
     * @brief indexOf
     * Index of the given Window in the model
     * @param window
     * @return
     */
    int indexOf(AbstractWindow * window);

public slots:

    /**
     * @brief addTask
     * Add a task to the model
     * @param surface
     */
    void addTask(AbstractWindow* window);

    /**
     * @brief removeTask
     * Removes a task from the model
     * @param index
     */
    void removeTask(const int &index);

signals:
    void countChanged();

private:
    Zpaces* m_zpaces;
    QVector<AbstractWindow*> m_windows;

    bool indexIsValid(const int &index) const;


};

#endif // TASKSMODEL_H
