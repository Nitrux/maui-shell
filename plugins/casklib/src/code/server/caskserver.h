#ifndef CASKSERVER_H
#define CASKSERVER_H

#include <QObject>

class CaskServer : public QObject
{
    Q_OBJECT
public:
    explicit CaskServer(QObject *parent = nullptr);

signals:

};

#endif // CASKSERVER_H
