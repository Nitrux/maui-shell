#ifndef APPSMODEL_H
#define APPSMODEL_H

#include <QObject>
#include <MauiKit3/Core/mauilist.h>

class AppsModel : public MauiList
{
    Q_OBJECT
    Q_PROPERTY(QString group READ group WRITE setGroup NOTIFY groupChanged)
    Q_PROPERTY(QVariantList groups READ groups)
    Q_PROPERTY(int limit READ limit WRITE setLimit NOTIFY limitChanged)

public:
    AppsModel(QObject *parent = nullptr);
    const FMH::MODEL_LIST &items() const override final;
    void componentComplete() override final;

    QString group() const;

    QVariantList groups() const;

    int limit() const;

public slots:
    void setGroup( QString group);

    void setLimit(int limit);

signals:
    void groupChanged( );

    void limitChanged();

private:
    FMH::MODEL_LIST m_data;
    void setList();

    QStringList m_groups;
    int m_limit = 99999;
    QString m_group;
};

#endif // APPSMODEL_H
