#pragma once
#include <QObject>
#include "casklib_export.h"

class CaskPower;
class CaskScreenshot;
class CaskChrome;

class DropShadowHelper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString appId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(int radius READ radius NOTIFY radiusChanged FINAL)

private:
    QString m_id;

    int m_radius;

    CaskChrome *m_chrome;

public:
    explicit DropShadowHelper(QObject * parent = nullptr);

    QString id() const;
    int radius() const;

    void setId(QString id);

Q_SIGNALS:
    void idChanged(QString id);
    void radiusChanged(int radius);
};

class CASKLIB_EXPORT CaskServer : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(CaskServer)

    Q_PROPERTY(CaskPower* power READ power CONSTANT FINAL)
    Q_PROPERTY(CaskScreenshot* screenshot READ screenshot CONSTANT FINAL)
    Q_PROPERTY(CaskChrome* chrome READ chrome CONSTANT FINAL)

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
    CaskScreenshot* screenshot();
    CaskChrome *chrome();

private:
    inline static CaskServer *m_instance = nullptr;
    explicit CaskServer(QObject *parent = nullptr);

    CaskPower *m_power;
    CaskScreenshot *m_screenshot;
    CaskChrome *m_chrome;

signals:

};

