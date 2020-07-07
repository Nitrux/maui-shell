#ifndef APPITEM_H
#define APPITEM_H

#include <QObject>

class AppItem : public QObject
{
    Q_OBJECT
public:
    explicit AppItem(QObject *parent = nullptr);

signals:

};

#endif // APPITEM_H
