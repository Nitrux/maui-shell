/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2008 Dmitry Suzdalev <dimsuz@gmail.com>
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

#include <QtCore/QDebug>

#include "notificationsimageutils.h"

inline void copyLineRGB32(QRgb *dst, const char *src, int width)
{
    const char *end = src + width * 3;
    for (; src != end; ++dst, src += 3)
        * dst = qRgb(src[0], src[1], src[2]);
}

inline void copyLineARGB32(QRgb *dst, const char *src, int width)
{
    const char *end = src + width * 4;
    for (; src != end; ++dst, src += 4)
        * dst = qRgba(src[0], src[1], src[2], src[3]);
}

QImage decodeImageHint(const QDBusArgument &arg)
{
    int width, height, stride, hasAlpha, bitsPerSample, channels;
    QByteArray pixels;

    // Decode hint
    arg.beginStructure();
    arg >> width >> height >> stride >> hasAlpha
        >> bitsPerSample >> channels >> pixels;
    arg.endStructure();

    char *ptr, *end;

    // Sanity check
    if ((width <= 0) || (width >= 2048) || (height <= 0) ||
            (height >= 2048) || (stride <= 0)) {
        qWarning() << "Image hint is not valid!";
        return QImage();
    }

    QImage::Format format = QImage::Format_Invalid;
    void (*function)(QRgb *, const char *, int) = 0;
    if (bitsPerSample == 8) {
        if (channels == 4) {
            format = QImage::Format_ARGB32;
            function = copyLineARGB32;
        } else if (channels == 3) {
            format = QImage::Format_RGB32;
            function = copyLineRGB32;
        }
    }

    if (format == QImage::Format_Invalid) {
        qWarning() << "Unsupported image format received from hint (hasAlpha:"
                   << hasAlpha << "bitsPerSample:" << bitsPerSample
                   << "channels:" << channels << ")";
        return QImage();
    }

    QImage image(width, height, format);
    ptr = pixels.data();
    end = ptr + pixels.length();
    for (int y = 0; y < height; ++y, ptr += stride) {
        if (ptr + channels * width > end) {
            qWarning() << "Image data is incomplete. y:" << y << "height:" << height;
            break;
        }
        function((QRgb *)(void *)image.scanLine(y), ptr, width);
    }

    return image;
}
