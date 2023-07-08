#pragma once
#include <QObject>
#include <QQuickItem>

#include "code/models/surfacesmodel.h"


class AbstractWindow;
class ZpacesModel;
class Zpaces;
class Zpace : public QObject
{
    Q_OBJECT
    Q_PROPERTY(SurfacesModel* windows READ windows FINAL CONSTANT)
    Q_PROPERTY(int limit READ limit WRITE setLimit NOTIFY limitChanged)
    Q_PROPERTY(QString wallpaper READ wallpaper WRITE setWallpaper NOTIFY wallpaperChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QQuickItem* control READ control WRITE setControl NOTIFY controlChanged)

public:
    explicit Zpace(ZpacesModel *parent = nullptr);
    ~Zpace();

    /**
     * @brief surfaces
     * Model of surfaces in this Zpace
     * @return
     */
    SurfacesModel *windows() const;

    /**
     * @brief acceptsSurface
     * Checks if the given surface can be accepted into this Zpace. This takes into consideration the limit of surfaces allowed by the Zpace, or if the surface is a widget or dialog, and it can be fitted.
     * @param surface
     * @return
     */
    bool acceptsWindow(AbstractWindow *window);

    /**
     * @brief addSurface
     * Adds the surface to this Zpace
     * @param surface
     */
    void addWindow(AbstractWindow *window);

    AbstractWindow* popWindow(AbstractWindow *window);

    int limit() const;

    const QString &wallpaper() const;
    void setWallpaper(const QString &newWallpaper);

    const QString &title() const;
    void setTitle(const QString &newTitle);

    QQuickItem* control() const;

public Q_SLOTS:
    void setLimit(int limit);

    void setControl(QQuickItem* control);

private:
    SurfacesModel* m_windowsModel;
    ZpacesModel* m_zpacesModelRoot;
    Zpaces * m_zpacesRoot;
    int m_limit = 20;

    void setConstrains();

    QString m_wallpaper = QStringLiteral("qrc:/wallpapers/maui_shell_dev_bg.png");

    QString m_title;

    QQuickItem* m_control;

Q_SIGNALS:
    void limitChanged(int limit);
    void closed();
    void wallpaperChanged();
    void titleChanged();
    void controlChanged(QQuickItem* control);
};

