#ifndef ENVIROMENT_H
#define ENVIROMENT_H

#include <QObject>

class Enviroment : public QObject
{
    Q_OBJECT
public:
    explicit Enviroment(QObject *parent = nullptr);

public slots:
    static void putenv(QString key, QString value);
    static void setIsMobile(const bool &value);
signals:

};

#endif // ENVIROMENT_H
