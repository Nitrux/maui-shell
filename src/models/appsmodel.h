#ifndef APPSMODEL_H
#define APPSMODEL_H

#include <QObject>

class AppsModel : public QObject
{
    Q_OBJECT
public:
    explicit AppsModel(QObject *parent = nullptr);

signals:

};

#endif // APPSMODEL_H
