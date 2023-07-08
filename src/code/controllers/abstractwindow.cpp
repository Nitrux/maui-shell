#include "abstractwindow.h"

void AbstractWindow::setIsActive(bool value)
{
    if(m_isActive == value)
    {
        return;
    }

    m_isActive = value;
    Q_EMIT this->activatedChanged(m_isActive);
}

AbstractWindow::AbstractWindow() : QObject(nullptr)
  ,m_chrome(nullptr)
{

}

void AbstractWindow::minimize()
{
    m_minimized = true;
    Q_EMIT minimizedChanged();
    Q_EMIT setMinimized();
}

void AbstractWindow::unminimize()
{
    m_minimized = false;
    Q_EMIT minimizedChanged();
    Q_EMIT unsetMinimized();
}

void AbstractWindow::maximize()
{
    Q_EMIT setMaximized();
}

void AbstractWindow::unmaximize()
{
    Q_EMIT unsetMaximized();
}

void AbstractWindow::sendFullscreen()
{
    Q_EMIT setFullscreen();
}

void AbstractWindow::unfullscreen()
{
    Q_EMIT unsetFullscreen();
}

void AbstractWindow::setChrome(QQuickItem *chrome)
{
    if (m_chrome == chrome)
        return;

    m_chrome = chrome;
    Q_EMIT chromeChanged(m_chrome);
}

bool AbstractWindow::minimized() const
{
    return m_minimized;
}

QQuickItem *AbstractWindow::chrome() const
{
    return m_chrome;
}

bool AbstractWindow::isActive() const
{
    return m_isActive;
}
