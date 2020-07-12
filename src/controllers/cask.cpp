#include "cask.h"

Cask::Cask(QObject *parent) : QObject(parent)
  , m_zpaces( new ZpaceModel( this ))
  , m_output(nullptr)
{

}

ZpaceModel *Cask::zpaces() const
{
    return m_zpaces;
}

QWaylandOutput *Cask::output() const
{
    return m_output;
}

void Cask::insertSurface(QWaylandXdgSurface *surface)
{
    m_zpaces->insert(surface);
}

void Cask::setOutput(QWaylandOutput *output)
{
    qDebug() << "Setting the outout" << output->geometry().width();
    if (m_output == output)
        return;

    m_output = output;
    connect(m_output, &QWaylandOutput::availableGeometryChanged, [&]()
    {
        qDebug() << "Avaliable geometry changed" << m_output->availableGeometry();
    });
    emit outputChanged(m_output);
}
