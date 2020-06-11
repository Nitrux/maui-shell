#include "hoverarea.h"

HoverArea::HoverArea()
{
    setAcceptHoverEvents(true);
    setAcceptedMouseButtons(Qt::NoButton);
}

void HoverArea::hoverMoveEvent(QHoverEvent* event)
{
    QQuickItem::hoverMoveEvent(event);
//    qDebug() << event->pos();
    if (!m_hovered) {
        m_hovered = true;
        emit entered();
    }
}

void HoverArea::hoverLeaveEvent(QHoverEvent *event)
{
    QQuickItem::hoverLeaveEvent(event);
//    qDebug() << event->pos();
    if (m_hovered) {
        m_hovered = false;
        emit exited();
    }
}
