#include "abstractwindow.h"

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

bool AbstractWindow::minimized() const
{
    return m_minimized;
}
