/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:LGPLv3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#include <QtGui/QIcon>

#include "notificationimagesstorage.h"
#include "notificationsdaemon.h"
#include "notificationsimageprovider.h"

NotificationsImageProvider::NotificationsImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Pixmap)
{
}

QPixmap NotificationsImageProvider::requestPixmap(const QString &id, QSize *realSize, const QSize &requestedSize)
{
    // Sanitize requested size
    QSize size(requestedSize);
    if (size.width() < 1)
        size.setWidth(1);
    if (size.height() < 1)
        size.setHeight(1);

    // Return real size
    if (realSize)
        *realSize = size;

    // Convert identifier (the second part is just a timestamp to make sure Image
    // is always reloaded)
    QString realId = id.split(QLatin1Char('/'), QString::SkipEmptyParts).at(0);
    bool ok = false;
    int notificationId = realId.toInt(&ok);
    if (!ok)
        return QPixmap();

    // Get image from the daemon
    NotificationImage *image = NotificationImagesStorage::instance()->get(notificationId);
    if (!image)
        return QPixmap();

    // Pixmap
    if (!image->image.isNull() && realSize) {
        *realSize = image->image.size();
        return image->image;
    }

    // Icon
    if (!image->iconName.isEmpty()) {
        QIcon icon = QIcon::fromTheme(image->iconName);
        if (!icon.isNull())
            return icon.pixmap(size);
    }

    // Application icon
    if (!image->entryIconName.isEmpty()) {
        QIcon icon = QIcon::fromTheme(image->entryIconName);
        if (!icon.isNull())
            return icon.pixmap(size);
    }

    // Default icon
    return QIcon::fromTheme(QStringLiteral("dialog-information")).pixmap(size);
}
