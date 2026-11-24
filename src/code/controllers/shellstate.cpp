#include "shellstate.h"

ShellState::ShellState(QObject *parent) : QObject(parent)
{
}

QObject *ShellState::win() const
{
    return m_win;
}

int ShellState::formFactor() const
{
    return m_formFactor;
}

bool ShellState::isMobile() const
{
    return m_isMobile;
}

QRectF ShellState::availableGeometry() const
{
    return m_availableGeometry;
}

QObject *ShellState::zpaces() const
{
    return m_zpaces;
}

QObject *ShellState::cask() const
{
    return m_cask;
}

QObject *ShellState::dock() const
{
    return m_dock;
}

QObject *ShellState::notifications() const
{
    return m_notifications;
}

QObject *ShellState::appsDB() const
{
    return m_appsDB;
}

QObject *ShellState::screenshot() const
{
    return m_screenshot;
}

QObject *ShellState::workspaces() const
{
    return m_workspaces;
}

QObject *ShellState::screen() const
{
    return m_screen;
}

QObject *ShellState::appsOverview() const
{
    return m_appsOverview;
}

void ShellState::setWorkspaces(QObject *workspaces)
{
    if (m_workspaces == workspaces)
        return;

    m_workspaces = workspaces;
    Q_EMIT workspacesChanged();
}

void ShellState::setScreen(QObject *screen)
{
    if (m_screen == screen)
        return;

    m_screen = screen;
    Q_EMIT screenChanged();
}

void ShellState::setAppsOverview(QObject *appsOverview)
{
    if (m_appsOverview == appsOverview)
        return;

    m_appsOverview = appsOverview;
    Q_EMIT appsOverviewChanged();
}

void ShellState::setWin(QObject *win)
{
    if (m_win == win)
        return;

    m_win = win;
    Q_EMIT winChanged();
}

void ShellState::setFormFactor(int formFactor)
{
    if (m_formFactor == formFactor)
        return;

    m_formFactor = formFactor;
    Q_EMIT formFactorChanged();
}

void ShellState::setIsMobile(bool isMobile)
{
    if (m_isMobile == isMobile)
        return;

    m_isMobile = isMobile;
    Q_EMIT isMobileChanged();
}

void ShellState::setAvailableGeometry(const QRectF &availableGeometry)
{
    if (m_availableGeometry == availableGeometry)
        return;

    m_availableGeometry = availableGeometry;
    Q_EMIT availableGeometryChanged();
}

void ShellState::setZpaces(QObject *zpaces)
{
    if (m_zpaces == zpaces)
        return;

    m_zpaces = zpaces;
    Q_EMIT zpacesChanged();
}

void ShellState::setCask(QObject *cask)
{
    if (m_cask == cask)
        return;

    m_cask = cask;
    Q_EMIT caskChanged();
}

void ShellState::setDock(QObject *dock)
{
    if (m_dock == dock)
        return;

    m_dock = dock;
    Q_EMIT dockChanged();
}

void ShellState::setNotifications(QObject *notifications)
{
    if (m_notifications == notifications)
        return;

    m_notifications = notifications;
    Q_EMIT notificationsChanged();
}

void ShellState::setAppsDB(QObject *appsDB)
{
    if (m_appsDB == appsDB)
        return;

    m_appsDB = appsDB;
    Q_EMIT appsDBChanged();
}

void ShellState::setScreenshot(QObject *screenshot)
{
    if (m_screenshot == screenshot)
        return;

    m_screenshot = screenshot;
    Q_EMIT screenshotChanged();
}
