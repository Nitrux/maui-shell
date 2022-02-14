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

#ifndef POLKITAGENTLISTENER_H
#define POLKITAGENTLISTENER_H

#include <QObject>
#include <QString>

#include <PolkitQt1/Identity>
#include <PolkitQt1/Details>
#include <PolkitQt1/Agent/Listener>
#include <PolkitQt1/Agent/Session>

class Dialog;
class PolKitAgentListener : public PolkitQt1::Agent::Listener
{
    Q_OBJECT

public:
    explicit PolKitAgentListener(QObject *parent = nullptr);

public slots:
    void initiateAuthentication(const QString &actionId,
                                const QString &message,
                                const QString &iconName,
                                const PolkitQt1::Details &details,
                                const QString &cookie,
                                const PolkitQt1::Identity::List &identities,
                                PolkitQt1::Agent::AsyncResult *result) override final;

    bool initiateAuthenticationFinish() override final { return true; }
    void cancelAuthentication() override final {}

    void request(const QString &request, bool echo);
    void completed(bool gainedAuthorization);

private:
    QHash<PolkitQt1::Agent::Session *,PolkitQt1::Identity> m_sessionIdentity;
    Dialog *m_dialog;

signals:
    void authenticationRequest(Dialog *dialog);
};

#endif // POLKITAGENTLISTENER_H
