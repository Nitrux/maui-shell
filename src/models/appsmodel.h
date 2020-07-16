#ifndef APPSMODEL_H
#define APPSMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QWaylandXdgSurface>


class AppsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles
    {
        Surface,
        Title,
        Index,
        Count
    };
    explicit AppsModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void insert(QWaylandXdgSurface *surface);

    int surfaceIndex(const QWaylandXdgSurface *surface) const;

public slots:
    void remove(const int &index);

private:
    QVector<QWaylandXdgSurface*> m_surfaces;
};

#endif // APPSMODEL_H
