/*
 * SPDX-FileCopyrightText: 2009 Nokia Corporation and/or its subsidiary(-ies).
 * SPDX-FileCopyrightText: 2016 David Edmundson <davidedmundson@kde.org>
 * SPDX-FileContributor: Marius Vollmer <marius.vollmer@nokia.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <QByteArray>
#include <QDebug>
#include <QString>
#include <QVariant>

#include "gconfitem.h"

#include <gconf/gconf-client.h>
#include <gconf/gconf-value.h>
#include <glib.h>

struct GConfItemPrivate {
    QString root;
    QVariant value;
    guint notify_id;

    static void notify_trampoline(GConfClient *, guint, GConfEntry *, gpointer);
};

#define withClient(c) for (GConfClient *c = (gconf_client_get_default()); c; g_object_unref(c), c = NULL)

static QByteArray convertKey(QString key)
{
    if (key.startsWith('/')) {
        return key.toUtf8();
    } else {
        qWarning() << "Using dot-separated key names with GConfItem is deprecated.";
        qWarning() << "Please use" << '/' + key.replace('.', '/') << "instead of" << key;
        return '/' + key.replace('.', '/').toUtf8();
    }
}

static QString convertKey(const char *key)
{
    return QString::fromUtf8(key);
}

static QVariant convertValue(GConfValue *src)
{
    if (!src) {
        return QVariant();
    } else {
        switch (src->type) {
        case GCONF_VALUE_INVALID:
            return QVariant(QVariant::Invalid);
        case GCONF_VALUE_BOOL:
            return QVariant((bool)gconf_value_get_bool(src));
        case GCONF_VALUE_INT:
            return QVariant(gconf_value_get_int(src));
        case GCONF_VALUE_FLOAT:
            return QVariant(gconf_value_get_float(src));
        case GCONF_VALUE_STRING:
            return QVariant(QString::fromUtf8(gconf_value_get_string(src)));
        case GCONF_VALUE_LIST:
            switch (gconf_value_get_list_type(src)) {
            case GCONF_VALUE_STRING: {
                QStringList result;
                for (GSList *elts = gconf_value_get_list(src); elts; elts = elts->next)
                    result.append(QString::fromUtf8(gconf_value_get_string((GConfValue *)elts->data)));
                return QVariant(result);
            }
            default: {
                QList<QVariant> result;
                for (GSList *elts = gconf_value_get_list(src); elts; elts = elts->next)
                    result.append(convertValue((GConfValue *)elts->data));
                return QVariant(result);
            }
            }
        case GCONF_VALUE_SCHEMA:
        default:
            return QVariant();
        }
    }
}

static GConfValue *convertString(const QString &str)
{
    GConfValue *v = gconf_value_new(GCONF_VALUE_STRING);
    gconf_value_set_string(v, str.toUtf8().data());
    return v;
}

static GConfValueType primitiveType(const QVariant &elt)
{
    switch (elt.type()) {
    case QVariant::String:
        return GCONF_VALUE_STRING;
    case QVariant::Int:
        return GCONF_VALUE_INT;
    case QVariant::Double:
        return GCONF_VALUE_FLOAT;
    case QVariant::Bool:
        return GCONF_VALUE_BOOL;
    default:
        return GCONF_VALUE_INVALID;
    }
}

static GConfValueType uniformType(const QList<QVariant> &list)
{
    GConfValueType result = GCONF_VALUE_INVALID;

    Q_FOREACH (const QVariant &elt, list) {
        GConfValueType elt_type = primitiveType(elt);

        if (elt_type == GCONF_VALUE_INVALID)
            return GCONF_VALUE_INVALID;

        if (result == GCONF_VALUE_INVALID)
            result = elt_type;
        else if (result != elt_type)
            return GCONF_VALUE_INVALID;
    }

    if (result == GCONF_VALUE_INVALID)
        return GCONF_VALUE_STRING; // empty list.
    else
        return result;
}

static int convertValue(const QVariant &src, GConfValue **valp)
{
    GConfValue *v;

    switch (src.type()) {
    case QVariant::Invalid:
        v = nullptr;
        break;
    case QVariant::Bool:
        v = gconf_value_new(GCONF_VALUE_BOOL);
        gconf_value_set_bool(v, src.toBool());
        break;
    case QVariant::Int:
        v = gconf_value_new(GCONF_VALUE_INT);
        gconf_value_set_int(v, src.toInt());
        break;
    case QVariant::Double:
        v = gconf_value_new(GCONF_VALUE_FLOAT);
        gconf_value_set_float(v, src.toDouble());
        break;
    case QVariant::String:
        v = convertString(src.toString());
        break;
    case QVariant::StringList: {
        GSList *elts = nullptr;
        v = gconf_value_new(GCONF_VALUE_LIST);
        gconf_value_set_list_type(v, GCONF_VALUE_STRING);
        Q_FOREACH (const QString &str, src.toStringList())
            elts = g_slist_prepend(elts, convertString(str));
        gconf_value_set_list_nocopy(v, g_slist_reverse(elts));
        break;
    }
    case QVariant::List: {
        GConfValueType elt_type = uniformType(src.toList());
        if (elt_type == GCONF_VALUE_INVALID)
            v = nullptr;
        else {
            GSList *elts = nullptr;
            v = gconf_value_new(GCONF_VALUE_LIST);
            gconf_value_set_list_type(v, elt_type);
            Q_FOREACH (const QVariant &elt, src.toList()) {
                GConfValue *val = nullptr;
                convertValue(elt, &val); // guaranteed to succeed.
                elts = g_slist_prepend(elts, val);
            }
            gconf_value_set_list_nocopy(v, g_slist_reverse(elts));
        }
        break;
    }
    default:
        return 0;
    }

    *valp = v;
    return 1;
}

void GConfItemPrivate::notify_trampoline(GConfClient *, guint, GConfEntry *entry, gpointer data)
{
    GConfItem *item = (GConfItem *)data;

    item->update_value(true, entry->key, convertValue(entry->value));
}

void GConfItem::update_value(bool emit_signal, const QString &key, const QVariant &value)
{
    QVariant new_value;

    if (emit_signal) {
        subtreeChanged(key, value);
    }
}

QString GConfItem::root() const
{
    return priv->root;
}

QVariant GConfItem::value(const QString &subKey) const
{
    QVariant new_value;
    withClient(client)
    {
        GError *error = nullptr;
        QByteArray k = convertKey(priv->root + '/' + subKey);
        GConfValue *v = gconf_client_get(client, k.data(), &error);

        if (error) {
            qWarning() << error->message;
            g_error_free(error);
            new_value = QVariant();
        } else {
            new_value = convertValue(v);
            if (v)
                gconf_value_free(v);
        }
    }
    return new_value;
}

void GConfItem::set(const QString &subKey, const QVariant &val)
{
    withClient(client)
    {
        QByteArray k = convertKey(priv->root + '/' + subKey);
        GConfValue *v;
        if (convertValue(val, &v)) {
            GError *error = nullptr;

            if (v) {
                gconf_client_set(client, k.data(), v, &error);
                gconf_value_free(v);
            } else {
                gconf_client_unset(client, k.data(), &error);
            }

            if (error) {
                qWarning() << error->message;
                g_error_free(error);
            }
        } else {
            qWarning() << "Can't store a" << val.typeName();
        }
    }
}

QList<QString> GConfItem::listDirs() const
{
    QList<QString> children;

    withClient(client)
    {
        QByteArray k = convertKey(priv->root);
        GSList *dirs = gconf_client_all_dirs(client, k.data(), nullptr);
        for (GSList *d = dirs; d; d = d->next) {
            children.append(convertKey((char *)d->data));
            g_free(d->data);
        }
        g_slist_free(dirs);
    }

    return children;
}

QList<QString> GConfItem::listEntries() const
{
    QList<QString> children;

    withClient(client)
    {
        QByteArray k = convertKey(priv->root);
        GSList *entries = gconf_client_all_entries(client, k.data(), nullptr);
        for (GSList *e = entries; e; e = e->next) {
            children.append(convertKey(((GConfEntry *)e->data)->key));
            gconf_entry_free((GConfEntry *)e->data);
        }
        g_slist_free(entries);
    }

    return children;
}

GConfItem::GConfItem(const QString &key, QObject *parent)
    : QObject(parent)
    , priv(new GConfItemPrivate)
{
    priv->root = key;
    withClient(client)
    {
        QByteArray k = convertKey(priv->root);
        gconf_client_add_dir(client, k.data(), GCONF_CLIENT_PRELOAD_ONELEVEL, nullptr);
        priv->notify_id = gconf_client_notify_add(client, k.data(), GConfItemPrivate::notify_trampoline, this, nullptr, nullptr);
    }
}

GConfItem::~GConfItem()
{
    withClient(client)
    {
        QByteArray k = convertKey(priv->root);
        gconf_client_notify_remove(client, priv->notify_id);
        gconf_client_remove_dir(client, k.data(), nullptr);
    }
    delete priv;
}
