#include <QObject>

class AbstractWindow;
class TasksModel;
class Task : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY nameChanged FINAL)
    Q_PROPERTY(QString path READ path NOTIFY pathChanged FINAL)
    Q_PROPERTY(QString iconName READ iconName NOTIFY iconNameChanged FINAL)
    Q_PROPERTY(QString fileName READ fileName NOTIFY fileNameChanged FINAL)
    Q_PROPERTY(QString executable READ executable NOTIFY executableChanged FINAL)
    Q_PROPERTY(AbstractWindow* window READ window NOTIFY windowChanged FINAL)
    Q_PROPERTY(bool isPinned READ isPinned WRITE setIsPinned NOTIFY isPinnedChanged)

public:
    explicit Task(const QString &id = QString(), AbstractWindow *window = nullptr, TasksModel *modelRoot = nullptr);

    AbstractWindow *window();

    void setWindow(AbstractWindow *window);
    void setId(const QString &id);

    QString name() const;
    QString iconName() const;
    QString fileName() const;
    QString id() const;

    void setName(const QString &name);
    void setIconName(const QString &name);
    void setPath(const QString &path);

    QString executable() const;

    bool isPinned() const;

    QString path() const;

public Q_SLOTS:
    void setIsPinned(bool isPinned);

private:
    /**
 * @brief m_id
 * It can be the app ID or the desktop file path
 */
    QString m_id;
    AbstractWindow *m_window;
    TasksModel *m_modelRoot;
    QString m_name;
    QString m_iconName;
    QString m_fileName;

    QString m_executable;

    bool m_isPinned;

    void setWindowConnections();
    void setData();
    void setFileName(const QString &name);
    void setExecutable(const QString &exec);

    void pinTask();
    void unPinTask();

    QString m_path;

Q_SIGNALS:
    void windowChanged();
    void idChanged();
    void nameChanged(QString name);
    void iconNameChanged(QString iconName);
    void fileNameChanged();
    void executableChanged(QString executable);
    void isPinnedChanged(bool isPinned);
    void closed();
    void pathChanged(QString path);
};
