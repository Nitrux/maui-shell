#pragma once

#include <QObject>
#include <MauiKit4/Core/mauilist.h>

class AppsModel : public MauiList
{
    Q_OBJECT
    Q_PROPERTY(QString group READ group WRITE setGroup NOTIFY groupChanged)
    Q_PROPERTY(QVariantList groups READ groups FINAL CONSTANT)
    Q_PROPERTY(int limit READ limit WRITE setLimit NOTIFY limitChanged)

public:
    AppsModel(QObject *parent = nullptr);
    const FMH::MODEL_LIST &items() const override final;
    void componentComplete() override final;

    QString group() const;

    QVariantList groups() const;

    int limit() const;

public Q_SLOTS:
    void setGroup( QString group);

    void setLimit(int limit);

Q_SIGNALS:
    void groupChanged( );

    void limitChanged();

private:
    FMH::MODEL_LIST m_data;
    void setList();

    QStringList m_groups;
    int m_limit = 99999;
    QString m_group;
};
