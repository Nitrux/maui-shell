/*
    SPDX-FileCopyrightText: 2011 Sebastian Kügler <sebas@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "powermanagementservice.h"
#include "powermanagementjob.h"

PowerManagementService::PowerManagementService(QObject *parent)
    : Plasma::Service(parent)
{
    setName(QStringLiteral("powermanagementservice"));
}

ServiceJob *PowerManagementService::createJob(const QString &operation, QMap<QString, QVariant> &parameters)
{
    return new PowerManagementJob(operation, parameters, this);
}
