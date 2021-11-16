#ifndef STACKABLEITEM_H
#define STACKABLEITEM_H

#include <QQuickItem>

class StackableItem : public QQuickItem
{
    Q_OBJECT
public:
    StackableItem();

public Q_SLOTS:
    void raise();
    void lower();
};

#endif // STACKABLEITEM_H
