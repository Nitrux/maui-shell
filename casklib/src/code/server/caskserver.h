#pragma once
#include <QObject>
#include "casklib_export.h"
#include <CaskServer/caskpower.h>

class CASKLIB_EXPORT CaskServer : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(CaskServer)

    Q_PROPERTY(CaskPower* power READ power CONSTANT FINAL)

public:
    static CaskServer *instance()
    {
        if(!m_instance)
        {
            m_instance = new CaskServer();
        }

        return m_instance;
    }
    CaskPower *power();

private:
    inline static CaskServer *m_instance = nullptr;
    explicit CaskServer(QObject *parent = nullptr);

    CaskPower *m_power;

signals:

};
