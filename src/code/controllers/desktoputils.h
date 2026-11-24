#pragma once

#include <QString>

#include <KService>

namespace DesktopUtils
{
inline QString normalizeId(const QString &id)
{
    if (id.isEmpty())
        return QString();

    QString normalized = id;
    if (!normalized.endsWith(QStringLiteral(".desktop")))
        normalized += QStringLiteral(".desktop");

    return normalized;
}

inline KService::Ptr serviceForId(const QString &id)
{
    const QString normalized = normalizeId(id);
    if (normalized.isEmpty())
        return KService::Ptr();

    return KService::serviceByStorageId(normalized);
}
}
