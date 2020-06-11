#include "iconprovider.h"
#include <qt5xdg/XdgIcon>

#include <QDebug>
#include <QDir>
#include <QDirIterator>

static QDir UsrSharePixmaps("/usr/share/pixmaps");

IconProvider::IconProvider()
  : QQuickImageProvider(QQuickImageProvider::Pixmap)
{
    XdgIcon::setThemeName(QStringLiteral("oxygen")); // TODO make configurable
    qDebug() << "theme is" << QIcon::themeName() << "paths" << QIcon::themeSearchPaths()
             << "default icon" << XdgIcon::defaultApplicationIconName();
}

QPixmap IconProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
//qDebug() << id << requestedSize;
    // absolute path: just load the image
    if (id.startsWith('/')) {
        QPixmap ret(id);
        if (ret.width() > requestedSize.width() || ret.height() > requestedSize.height())
            ret = ret.scaled(requestedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        if (size)
            *size = ret.size();
        return ret;
    }
    // main strategy: QIcon usually knows how to find it
    QIcon icon = QIcon::fromTheme(id);
//    QIcon icon = XdgIcon::fromTheme(id, XdgIcon::defaultApplicationIcon()); // often not working well
    // fall back to /usr/share/pixmaps if nothing found yet
    if (icon.isNull()) {
        QStringList p = UsrSharePixmaps.entryList({id + "*"}, QDir::Files);
        if (!p.isEmpty()) {
            QPixmap ret(UsrSharePixmaps.filePath(p.first()));
            if (ret.width() > requestedSize.width() || ret.height() > requestedSize.height())
                ret = ret.scaled(requestedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            if (size)
                *size = ret.size();
            return ret;
        }
    }
    // default app icon if all else fails
    if (icon.isNull()) {
        qWarning() << "failed to find icon" << id;
        icon = QIcon::fromTheme(XdgIcon::defaultApplicationIconName());
    }
    QPixmap ret = icon.pixmap(requestedSize);
    if (size)
        *size = ret.size();
    return ret;
}
