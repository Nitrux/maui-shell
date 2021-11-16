#ifndef HOVERAREA_H
#define HOVERAREA_H

#include <QQuickItem>

class HoverArea : public QQuickItem
{
    Q_OBJECT
public:
    HoverArea();
    void hoverMoveEvent(QHoverEvent *event) Q_DECL_OVERRIDE;
    void hoverLeaveEvent(QHoverEvent *event) Q_DECL_OVERRIDE;

signals:
    void entered();
    void exited();

private:
    bool m_hovered = false;
};

#endif // HOVERAREA_H
