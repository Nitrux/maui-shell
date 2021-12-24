/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "pulseobject.h"

#include "context.h"

#include <QIcon>

namespace QPulseAudio
{
PulseObject::PulseObject(QObject *parent)
    : QObject(parent)
    , m_index(0)
{
}

PulseObject::~PulseObject()
{
}

Context *PulseObject::context() const
{
    return Context::instance();
}

uint32_t PulseObject::index() const
{
    return m_index;
}

QString PulseObject::iconName() const
{
    QString name = m_properties.value(QStringLiteral("device.icon_name")).toString();
    if (!name.isEmpty() && QIcon::hasThemeIcon(name)) {
        return name;
    }

    name = m_properties.value(QStringLiteral("media.icon_name")).toString();
    if (!name.isEmpty() && QIcon::hasThemeIcon(name)) {
        return name;
    }

    name = m_properties.value(QStringLiteral("window.icon_name")).toString();
    if (!name.isEmpty() && QIcon::hasThemeIcon(name)) {
        return name;
    }

    name = m_properties.value(QStringLiteral("application.icon_name")).toString();
    if (!name.isEmpty() && QIcon::hasThemeIcon(name)) {
        return name;
    }

    name = m_properties.value(QStringLiteral("application.process.binary")).toString();
    if (!name.isEmpty() && QIcon::hasThemeIcon(name)) {
        return name;
    }

    name = m_properties.value(QStringLiteral("application.name")).toString();
    if (!name.isEmpty() && QIcon::hasThemeIcon(name)) {
        return name;
    }

    name = property("name").toString();
    if (!name.isEmpty() && QIcon::hasThemeIcon(name)) {
        return name;
    }

    return QString();
}

QVariantMap PulseObject::properties() const
{
    return m_properties;
}

} // QPulseAudio
