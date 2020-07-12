#ifndef CASK_H
#define CASK_H

#include <QObject>
#include "zpacemodel.h"
#include <QWaylandOutput>
#include <QWaylandXdgSurface>

class Cask : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ZpaceModel *zpaces READ zpaces CONSTANT FINAL)
    Q_PROPERTY(QWaylandOutput *output READ output WRITE setOutput NOTIFY outputChanged)
public:
    explicit Cask(QObject *parent = nullptr);

    ZpaceModel *zpaces() const;
    QWaylandOutput * output() const;

public slots:
    void insertSurface(QWaylandXdgSurface *surface);
    void setOutput(QWaylandOutput * output);

private:
    ZpaceModel *m_zpaces;
    QWaylandOutput * m_output;

signals:
    void outputChanged(QWaylandOutput * output);
};

#endif // CASK_H
