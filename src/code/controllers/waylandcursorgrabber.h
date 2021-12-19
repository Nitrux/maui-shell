// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef WAYLANDCURSORGRABBER_H
#define WAYLANDCURSORGRABBER_H

#include <QWaylandQuickItem>
#include <QWaylandSeat>
#include <QWaylandSurfaceGrabber>

class QWaylandDrag;
class WaylandCursorGrabber : public QWaylandQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QWaylandSeat *seat READ seat WRITE setSeat NOTIFY seatChanged)
    Q_PROPERTY(int hotspotX READ hotspotX NOTIFY hotspotXChanged)
    Q_PROPERTY(int hotspotY READ hotspotY NOTIFY hotspotYChanged)
    Q_PROPERTY(bool grab READ grab WRITE setGrab NOTIFY grabChanged)
public:
    WaylandCursorGrabber(QQuickItem *parent = nullptr);

    QWaylandSeat *seat() const;
    void setSeat(QWaylandSeat *seat);

    int hotspotX() const;
    int hotspotY() const;

    bool grab() const;
    void setGrab(bool value);

Q_SIGNALS:
    void seatChanged(QWaylandSeat *seat);
    void hotspotXChanged(int hotspotX);
    void hotspotYChanged(int hotspotY);
    void grabChanged(bool grab);

private:
    QWaylandSurfaceGrabber *m_grabber = nullptr;
    QWaylandSurfaceGrabber *m_dragIconGrabber = nullptr;
    QWaylandDrag *m_drag = nullptr;
    bool m_grabbing = false;
    QWaylandSeat *m_seat = nullptr;
    int m_hotspotX = -1;
    int m_hotspotY = -1;
    bool m_grab = false;

private Q_SLOTS:
    void handleCursorSurfaceRequest(QWaylandSurface *cursorSurface, int hotspotX, int hotspotY);
    void handleDragIconChanged();
    void handleRedraw();
    void handleGrab(const QImage &image);
};

#endif // WAYLANDCURSORGRABBER_H
