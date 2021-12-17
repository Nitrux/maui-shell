#ifndef TASKSMODEL_H
#define TASKSMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <QtWaylandCompositor/QWaylandShellSurface>

class Task;
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
        TaskItem
    };
    explicit TasksModel(Zpaces *parent = nullptr);
~TasksModel();
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
     * Index of the given Task in the model
     * @param window
     * @return
     */
    int indexOf(Task *task);

public slots:

    /**
     * @brief addTask
     * Add a task to the model based off an AbstractWindow
     * @param surface
     */
    void addTask(AbstractWindow* window);

    /**
     * @brief addTask
     * Add a task to the model based off an id such as the dekstop file path
     * @param surface
     */
    void addTask(const QString &id, const bool &pin = false);

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
    QVector<Task*> m_tasks;

    Task * findTask(const QString &id);
    bool indexIsValid(const int &index) const;


};

#endif // TASKSMODEL_H
