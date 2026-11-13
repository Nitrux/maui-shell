#pragma once

#include <QObject>
#include <QRectF>

/**
 * @brief The ShellState class
 *
 * Shell-wide state shared between the root QML files (main.qml /
 * CaskScreen.qml) and the components loaded from separate files. QML ids are
 * file-local, so components such as Chrome, Dock or PanelItem can not reach
 * the objects created in CaskScreen.qml directly. CaskApp exposes one instance
 * of this class as the `CaskShell` context property and the owning QML files fill
 * in the references.
 */
class ShellState : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QObject *win READ win WRITE setWin NOTIFY winChanged FINAL)
    Q_PROPERTY(int formFactor READ formFactor WRITE setFormFactor NOTIFY formFactorChanged FINAL)
    Q_PROPERTY(bool isMobile READ isMobile WRITE setIsMobile NOTIFY isMobileChanged FINAL)
    Q_PROPERTY(QRectF availableGeometry READ availableGeometry WRITE setAvailableGeometry NOTIFY availableGeometryChanged FINAL)
    Q_PROPERTY(QObject *zpaces READ zpaces WRITE setZpaces NOTIFY zpacesChanged FINAL)
    Q_PROPERTY(QObject *cask READ cask WRITE setCask NOTIFY caskChanged FINAL)
    Q_PROPERTY(QObject *dock READ dock WRITE setDock NOTIFY dockChanged FINAL)
    Q_PROPERTY(QObject *notifications READ notifications WRITE setNotifications NOTIFY notificationsChanged FINAL)
    Q_PROPERTY(QObject *appsDB READ appsDB WRITE setAppsDB NOTIFY appsDBChanged FINAL)
    Q_PROPERTY(QObject *screenshot READ screenshot WRITE setScreenshot NOTIFY screenshotChanged FINAL)
    Q_PROPERTY(QObject *workspaces READ workspaces WRITE setWorkspaces NOTIFY workspacesChanged FINAL)
    Q_PROPERTY(QObject *screen READ screen WRITE setScreen NOTIFY screenChanged FINAL)
    Q_PROPERTY(QObject *appsOverview READ appsOverview WRITE setAppsOverview NOTIFY appsOverviewChanged FINAL)

public:
    explicit ShellState(QObject *parent = nullptr);

    QObject *win() const;
    int formFactor() const;
    bool isMobile() const;
    QRectF availableGeometry() const;
    QObject *zpaces() const;
    QObject *cask() const;
    QObject *dock() const;
    QObject *notifications() const;
    QObject *appsDB() const;
    QObject *screenshot() const;
    QObject *workspaces() const;
    QObject *screen() const;
    QObject *appsOverview() const;

public Q_SLOTS:
    void setWin(QObject *win);
    void setFormFactor(int formFactor);
    void setIsMobile(bool isMobile);
    void setAvailableGeometry(const QRectF &availableGeometry);
    void setZpaces(QObject *zpaces);
    void setCask(QObject *cask);
    void setDock(QObject *dock);
    void setNotifications(QObject *notifications);
    void setAppsDB(QObject *appsDB);
    void setScreenshot(QObject *screenshot);
    void setWorkspaces(QObject *workspaces);
    void setScreen(QObject *screen);
    void setAppsOverview(QObject *appsOverview);

Q_SIGNALS:
    void winChanged();
    void formFactorChanged();
    void isMobileChanged();
    void availableGeometryChanged();
    void zpacesChanged();
    void caskChanged();
    void dockChanged();
    void notificationsChanged();
    void appsDBChanged();
    void screenshotChanged();
    void workspacesChanged();
    void screenChanged();
    void appsOverviewChanged();

private:
    QObject *m_win = nullptr;
    int m_formFactor = 0;
    bool m_isMobile = false;
    QRectF m_availableGeometry;
    QObject *m_zpaces = nullptr;
    QObject *m_cask = nullptr;
    QObject *m_dock = nullptr;
    QObject *m_notifications = nullptr;
    QObject *m_appsDB = nullptr;
    QObject *m_screenshot = nullptr;
    QObject *m_workspaces = nullptr;
    QObject *m_screen = nullptr;
    QObject *m_appsOverview = nullptr;
};
