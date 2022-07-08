#ifndef CASKSERVER_H
#define CASKSERVER_H

#include <QObject>
#include "casklib_export.h"

class PowerServer;
class CASKLIB_EXPORT CaskServer : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(CaskServer)

    Q_PROPERTY(PowerServer* power READ power CONSTANT FINAL)

public:

    bool init();

    static CaskServer *instance()
    {
        if(!m_instance)
        {
            m_instance = new CaskServer();
        }

        return m_instance;
    }
    PowerServer *power();

private:
    inline static CaskServer *m_instance = nullptr;
    explicit CaskServer(QObject *parent = nullptr);

    PowerServer *m_power;

signals:

};

#endif // CASKSERVER_H
