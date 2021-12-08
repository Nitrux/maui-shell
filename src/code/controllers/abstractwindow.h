#ifndef ABSTRACTWINDOW_H
#define ABSTRACTWINDOW_H

#include <QObject>
#include <QtWaylandCompositor/QWaylandShellSurface>

class AbstractWindow : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QString appId READ appId NOTIFY appIdChanged)
    Q_PROPERTY(QString iconName READ iconName NOTIFY iconNameChanged)
    Q_PROPERTY(QString appName READ appName NOTIFY appNameChanged)

public:
    explicit AbstractWindow(QObject *parent = nullptr);

    virtual QString title() const = 0;
    virtual QString appId() const = 0;
    virtual QString iconName() const = 0;
    virtual QString appName() const = 0;
    virtual QWaylandShellSurface * shellSurface() const = 0;

public slots:
    virtual void close() = 0;

signals:
    void titleChanged();
    void appIdChanged();
    void iconNameChanged();
    void appNameChanged();
};

#endif // ABSTRACTWINDOW_H
