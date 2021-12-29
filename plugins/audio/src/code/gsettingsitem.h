/*
 * SPDX-FileCopyrightText: 2018 Nicolas Fella <nicolas.fella@gmx.de>
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 */

#ifndef GSETTINGSITEM_H
#define GSETTINGSITEM_H

#include <QObject>
#include <QStringList>
#include <QVariant>

#include <gio/gio.h>

class GSettingsItem : public QObject
{
    Q_OBJECT

public:
    explicit GSettingsItem(const QString &key, QObject *parent = nullptr);
    virtual ~GSettingsItem() override;

    QVariant value(const QString &key) const;
    void set(const QString &key, const QVariant &val);

    bool isValid() const;

Q_SIGNALS:
    void subtreeChanged();

private:
    GSettings *m_settings = nullptr;

    static void settingChanged(GSettings *settings, const gchar *key, gpointer data)
    {
        Q_UNUSED(settings)
        Q_UNUSED(key)

        GSettingsItem *self = static_cast<GSettingsItem *>(data);
        Q_EMIT self->subtreeChanged();
    }
};

#endif // GCONFITEM_H
