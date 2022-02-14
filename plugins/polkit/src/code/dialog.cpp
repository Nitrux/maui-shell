/*
 * Copyright (C) 2021 CutefishOS Team.
 *
 * Author:     revenmartin <revenmartin@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "dialog.h"

#include <QQmlContext>
#include <QDebug>

Dialog::Dialog(const QString &action, const QString &message,
               const QString &cookie, const QString &identity,
               PolkitQt1::Agent::AsyncResult *result)
    : m_action(action)
    , m_message(message)
    , m_cookie(cookie)
    , m_identity(identity)
    , m_password(QString())
    , m_result(result)
{
    qDebug() << "Creating ConfirmationDialog";


}

Dialog::~Dialog()
{
    qDebug() << "Deleting ConfirmationDialog";

}

void Dialog::setConfirmationResult(const QString &passwd)
{
    m_password = passwd;

    emit finished(this);
}

