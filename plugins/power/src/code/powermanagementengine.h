/*
    SPDX-FileCopyrightText: 2007 Aaron Seigo <aseigo@kde.org>
    SPDX-FileCopyrightText: 2007-2008 Sebastian Kuegler <sebas@kde.org>
    SPDX-FileCopyrightText: 2008 Dario Freddi <drf54321@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#pragma once


#include <QDBusConnection>
#include <QHash>
#include <QPair>

using InhibitionInfo = QPair<QString, QString>;

class PowerManagementJob;
/**
 * This class provides runtime information about the battery and AC status
 * for use in power management Plasma applets.
 */
class PowermanagementEngine : public QObject
{
    Q_OBJECT

public:
    PowermanagementEngine(QObject *parent = nullptr);
    QStringList sources() const;
    bool sourceRequestEvent(const QString &name);


protected:
    bool updateSourceEvent(const QString &source);
    void init();

private Q_SLOTS:

    void inhibitionsChanged(const QList<InhibitionInfo> &added, const QStringList &removed);

private:
    PowerManagementJob *m_job;


    void setData(const QString &name, const QString &name2, const QVariant &value);


};
