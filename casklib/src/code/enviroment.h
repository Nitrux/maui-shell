#pragma once

#include <QObject>
#include <QRectF>

class Enviroment : public QObject
{
    Q_OBJECT
public:

    enum FormFactor : uint8_t
    {
        Phone,
        Tablet,
        Desktop
    }; Q_ENUM(FormFactor)

    explicit Enviroment(QObject *parent = nullptr);

public slots:
    static void putenv(QString, QString);
    static void setIsMobile(const bool &value);
    static QString appIconName(const QString &appId);
    static bool intersects(const QRectF first, const QRectF &second);
signals:

};

