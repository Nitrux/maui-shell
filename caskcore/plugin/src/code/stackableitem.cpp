#include "stackableitem.h"

StackableItem::StackableItem()
{
    this->setClip(false);
}

void StackableItem::lower()
{
    QQuickItem *parent = parentItem();
    if (!parent)
        return;

    const auto items = parent->childItems();
    if (items.isEmpty())
        return;

    QQuickItem *bottom = items.first();
    if (this != bottom)
        stackBefore(bottom);
}

void StackableItem::raise()
{
    QQuickItem *parent = parentItem();
    if (!parent)
        return;

    const auto items = parent->childItems();
    if (items.isEmpty())
        return;

    QQuickItem *top = items.last();
    if (this != top)
        stackAfter(top);
}
