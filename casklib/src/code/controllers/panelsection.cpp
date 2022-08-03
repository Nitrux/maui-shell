#include "panelsection.h"

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
      if (item) {
          item->parent()->parent(); //this shoudl be a PanelSection
      } else if (parent) {
          qDebug() << "StackView must be attached to an Item";
      }
}

PanelSectionAttached::~PanelSectionAttached()
{

}

PanelSection *PanelSectionAttached::section() const
{
    return nullptr;
}
