#ifndef ZPACE_H
#define ZPACE_H

#include <QObject>
#include <QWaylandXdgSurface>

#include "appsmodel.h"

class Zpace : public QObject
{
    Q_OBJECT
    Q_PROPERTY(AppsModel *surfaces READ appsModel CONSTANT FINAL)
    Q_PROPERTY(int count READ count NOTIFY countChanged CONSTANT)

public:
    explicit Zpace(QObject *parent = nullptr);
    AppsModel * appsModel() const;

    /**
     * @brief insert
     * insert a surface to the space
     * @param surface
     * a surface to be inserted
     * @return
     * if the max quoate has not been filled then the surface is taken other wise it returns false
     */
    bool insert(QWaylandXdgSurface *surface);

    /**
     * @brief isFull
     * if the space is full and can or can not take another surface
     * @return
     */
    bool isFull();

    /**
     * @brief maxQuota
     * maximum size of surfaces can take this space
     * @return
     * maximum size
     */
    int maxQuota() const;

    int count () const;

private:
    AppsModel * m_appsModel;
    bool m_isFull = false;
    int m_maxQuota = 2;

signals:
    void isFullChanged();
    void maxQuotaChanged();
    void surfaceInserted();
    void surfaceMoved();
    void surfaceRemoved();

    void countChanged(int count);
};

#endif // ZPACE_H
