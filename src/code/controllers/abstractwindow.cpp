#include "abstractwindow.h"

void AbstractWindow::setIsActive(bool value)
{
    if(m_isActive == value)
    {
        return;
    }

    m_isActive = value;
    emit this->isActiveChanged(m_isActive);
}

AbstractWindow::AbstractWindow() : QObject(nullptr)
{

}

void AbstractWindow::minimize()
{
    m_minimized = true;
    emit minimizedChanged();
    emit setMinimized();
}

void AbstractWindow::unminimize()
{
    m_minimized = false;
    emit minimizedChanged();
    emit unsetMinimized();
}

void AbstractWindow::maximize()
{
    emit setMaximized();
}

void AbstractWindow::unmaximize()
{
    emit unsetMaximized();
}

void AbstractWindow::sendFullscreen()
{
    emit setFullscreen();
}

void AbstractWindow::unfullscreen()
{
    emit unsetFullscreen();
}

void AbstractWindow::setChrome(QQuickItem *chrome)
{
    if (m_chrome == chrome)
        return;

    m_chrome = chrome;
    emit chromeChanged(m_chrome);
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
