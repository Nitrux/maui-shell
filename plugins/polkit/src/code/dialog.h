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

#pragma once

#include <QObject>
#include <QString>
#include <QVariant>

#include <PolkitQt1/Agent/Listener>

class Dialog : public QObject
{
    Q_OBJECT    

    Q_PROPERTY(QString message READ message NOTIFY changed)
    Q_PROPERTY(QString action READ action NOTIFY changed)
    Q_PROPERTY(QString cookie READ cookie NOTIFY changed)
    Q_PROPERTY(QString identity READ identity NOTIFY changed)
    Q_PROPERTY(QString password READ password NOTIFY changed)

public:
    explicit Dialog(const QString &action, const QString &message,
                    const QString &cookie, const QString &identity,
                    PolkitQt1::Agent::AsyncResult *result);
    ~Dialog();

    Q_INVOKABLE void setConfirmationResult(const QString &password = QString());

    QString message() { return m_message; }
    QString action() { return m_action; }
    QString cookie() { return m_cookie; }
    QString identity() { return m_identity; }
    QString password() { return m_password; }

    PolkitQt1::Agent::AsyncResult *result() { return m_result; }

Q_SIGNALS:
    // This signal is never emitted at the moment, as we don't change the
    // properties of this window dynamically for now
    void changed();

    // Send approval to the D-Bus helper daemon
    void finished(Dialog *dialog);

private:
    QString m_action;
    QString m_message;
    QVariantMap m_details;
    QString m_cookie;
    QString m_identity;
    QString m_password;
    PolkitQt1::Agent::AsyncResult *m_result;

};
