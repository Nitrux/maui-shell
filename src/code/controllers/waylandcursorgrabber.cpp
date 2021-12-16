// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QWaylandDrag>
#include <QWaylandOutput>
#include <QDebug>

#include "waylandcursorgrabber.h"

WaylandCursorGrabber::WaylandCursorGrabber(QQuickItem *parent)
    : QWaylandQuickItem(parent)
{
    setEnabled(false);
    setInputEventsEnabled(false);
    setVisible(false);
}

QWaylandSeat *WaylandCursorGrabber::seat() const
{
    return m_seat;
}

void WaylandCursorGrabber::setSeat(QWaylandSeat *seat)
{
    if (m_seat == seat)
        return;
    if (!seat) {
        qWarning() << "Cannot unset WaylandCursorGrabber::seat";
        return;
    }

    if (m_dragIconGrabber) {
        disconnect(m_dragIconGrabber, &QWaylandSurfaceGrabber::success,
                   this, &WaylandCursorGrabber::handleGrab);
        m_dragIconGrabber->deleteLater();
        m_dragIconGrabber = nullptr;
    }

    if (m_seat) {
        disconnect(m_seat->drag(), &QWaylandDrag::iconChanged,
                   this, &WaylandCursorGrabber::handleDragIconChanged);
        disconnect(m_seat, &QWaylandSeat::cursorSurfaceRequest,
                   this, &WaylandCursorGrabber::handleCursorSurfaceRequest);
        m_seat = nullptr;
    }

    m_seat = seat;
    Q_EMIT seatChanged(seat);

    connect(m_seat, &QWaylandSeat::cursorSurfaceRequest,
            this, &WaylandCursorGrabber::handleCursorSurfaceRequest);
    connect(m_seat->drag(), &QWaylandDrag::iconChanged,
            this, &WaylandCursorGrabber::handleDragIconChanged);

    m_dragIconGrabber = new QWaylandSurfaceGrabber(m_seat->drag()->icon());
    connect(m_dragIconGrabber, &QWaylandSurfaceGrabber::success,
            this, &WaylandCursorGrabber::handleGrab);
}

int WaylandCursorGrabber::hotspotX() const
{
    return m_hotspotX;
}

int WaylandCursorGrabber::hotspotY() const
{
    return m_hotspotY;
}

bool WaylandCursorGrabber::grab() const
{
    return m_grab;
}

void WaylandCursorGrabber::setGrab(bool value)
{
    if (m_grab != value) {
        m_grab = value;
        Q_EMIT grabChanged(value);
    }
}

void WaylandCursorGrabber::handleCursorSurfaceRequest(QWaylandSurface *cursorSurface, int hotspotX, int hotspotY)
{
    if (m_hotspotX != hotspotX) {
        m_hotspotX = hotspotX;
        Q_EMIT hotspotXChanged(hotspotX);
    }

    if (m_hotspotY == hotspotY) {
        m_hotspotY = hotspotY;
        Q_EMIT hotspotYChanged(hotspotY);
    }

    if (surface() == cursorSurface)
        return;

    if (m_grabber) {
        disconnect(m_grabber, &QWaylandSurfaceGrabber::success,
                   this, &WaylandCursorGrabber::handleGrab);
        m_grabber->deleteLater();
        m_grabber = nullptr;
    }

    if (surface())
    {
        disconnect(surface(), &QWaylandSurface::redraw,
                   this, &WaylandCursorGrabber::handleRedraw);
    }

    if (cursorSurface) {
        setSurface(cursorSurface);
        connect(cursorSurface, &QWaylandSurface::redraw,
                this, &WaylandCursorGrabber::handleRedraw);

        m_grabber = new QWaylandSurfaceGrabber(cursorSurface, this);
        connect(m_grabber, &QWaylandSurfaceGrabber::success,
                this, &WaylandCursorGrabber::handleGrab);
    }
}

void WaylandCursorGrabber::handleDragIconChanged()
{
    if (!m_seat->drag()->visible())
        return;

    if (m_grab && !m_grabbing && m_dragIconGrabber) {
        m_grabbing = true;
        m_dragIconGrabber->grab();
    }
}

void WaylandCursorGrabber::handleRedraw()
{
    if (!surface())
        return;
    if (!surface()->isCursorSurface() || !surface()->hasContent())
        return;
    if (m_seat->drag()->visible())
        return;

    if (m_grab && !m_grabbing && m_grabber) {
        m_grabbing = true;
        m_grabber->grab();
    }
}

void WaylandCursorGrabber::handleGrab(const QImage &image)
{
    if (m_grab) {
        QCursor cursor(QPixmap::fromImage(image), m_hotspotX, m_hotspotY);
        output()->window()->setCursor(cursor);
    }

    m_grabbing = false;
}
