#ifndef PANELSECTION_H
#define PANELSECTION_H

#include <QQuickItem>
#include <QQmlEngine>
#include <QObject>

class PanelSectionAttached;
class PanelSection : public QQuickItem
{
    Q_OBJECT

public:
    PanelSection(QQuickItem *parent = nullptr);
    static PanelSectionAttached *qmlAttachedProperties(QObject *object);
};

class PanelSectionAttached : public QObject
{
    Q_OBJECT
    Q_PROPERTY(PanelSection* section READ section NOTIFY sectionChanged FINAL)

public:
    explicit PanelSectionAttached(QObject *parent = nullptr);
      ~PanelSectionAttached();
    PanelSection *section() const;

private:
    Q_DISABLE_COPY(PanelSectionAttached)
signals:

    void sectionChanged();

};

QML_DECLARE_TYPE(PanelSection)
QML_DECLARE_TYPEINFO(PanelSection, QML_HAS_ATTACHED_PROPERTIES)

#endif // PANELSECTION_H
