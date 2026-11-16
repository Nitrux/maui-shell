#include "panelsection.h"

#include <QDebug>

PanelSection::PanelSection(QQuickItem *parent) : QQuickItem(parent)
{

}

PanelSectionAttached *PanelSection::qmlAttachedProperties(QObject *object)
{
    return new PanelSectionAttached(object);
}

PanelSectionAttached::PanelSectionAttached(QObject *parent)
{
    QQuickItem *item = qobject_cast<QQuickItem *>(parent);
    if (!item) {
        if (parent)
            qDebug() << "PanelSection attached property must be used on an Item";
        return;
    }

    if (!item->parent())
        return;

    m_section = qobject_cast<PanelSection *>(item->parent()->parent());
    if (!m_section)
        qDebug() << "PanelSection attached property is not inside a PanelSection";
}

PanelSectionAttached::~PanelSectionAttached()
{

}

PanelSection *PanelSectionAttached::section() const
{
    return m_section;
}
