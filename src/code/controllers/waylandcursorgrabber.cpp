// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QWaylandDrag>
#include <QWaylandOutput>
#include <QDebug>
#include <QWaylandClient>
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

    m_drag = seat->drag();

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

    connect(m_drag, &QWaylandDrag::dragStarted,[this]()
    {
        qDebug() << "DRAG STARTED??????" << m_drag->origin()->client()->processId() << m_drag->visible();
    });

//    connect(m_seat, &QWaylandSeat::mouseFocusChanged,[this](QWaylandView *newFocus, QWaylandView *oldFocus)
//    {
//        qDebug() << "FOCUS CHANGED" << newFocus << oldFocus << m_drag->visible();
//        if(m_drag->visible())
//        {
//            m_drag->dragMove(newFocus->surface(), QPointF{0,0});
//            m_drag->drop();
//        }
//    });


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

    qDebug() << "CURSOR SURFACE REQUESTED";
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
    qDebug() << "HANDLE DRAG ICON CHANGED" << m_seat->drag()->visible();
    if (!m_seat->drag()->visible())
    {
//        m_seat->drag()->drop();
//            output()->window()->setCursor(QCursor{Qt::WaitCursor});

        return;
    }

//    QCursor cursor(QPixmap::fromImage(QImage("/home/camilo/Coding/qml/nota/src/assets/48.png")), m_hotspotX, m_hotspotY);

//    output()->window()->setCursor(cursor);

//    output()->window()->setCursor(QCursor{Qt::WaitCursor});

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
    qDebug() << "HANDLE DRAG IMAGE";

    if (m_grab) {
        qDebug() << "HANDLE DRAG IMAGE" << m_grab;

        QCursor cursor(QPixmap::fromImage(image), m_hotspotX, m_hotspotY);
        output()->window()->setCursor(cursor);
    }

    m_grabbing = false;
}
