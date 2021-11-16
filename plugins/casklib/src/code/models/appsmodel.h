#ifndef APPSMODEL_H
#define APPSMODEL_H

#include <QObject>
#include <MauiKit/Core/mauilist.h>

class AppsModel : public MauiList
{
    Q_OBJECT
public:
    AppsModel(QObject *parent = nullptr);
    const FMH::MODEL_LIST &items() const override final;

private:
    FMH::MODEL_LIST m_data;
    void setList();
};

#endif // APPSMODEL_H
