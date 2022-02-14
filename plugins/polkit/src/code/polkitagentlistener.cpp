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

#include "polkitagentlistener.h"
#include "dialog.h"

#include <QDebug>

PolKitAgentListener::PolKitAgentListener(QObject *parent)
    : PolkitQt1::Agent::Listener(parent)
    , m_dialog(nullptr)
{
    qDebug() << "POLKIT STARTED";

}

void PolKitAgentListener::initiateAuthentication(const QString &actionId,
                                                 const QString &message,
                                                 const QString &iconName,
                                                 const PolkitQt1::Details &details,
                                                 const QString &cookie,
                                                 const PolkitQt1::Identity::List &identities,
                                                 PolkitQt1::Agent::AsyncResult *result)
{
   qDebug() << "POLKIT CALLED" << actionId << message;
    Q_UNUSED(iconName);
    Q_UNUSED(details);

    if (m_dialog) {
        m_dialog->deleteLater();
        m_dialog = nullptr;
    }

    m_dialog = new Dialog(actionId, message, cookie, identities.first().toString(), result);

    for (const PolkitQt1::Identity &i : identities) {
        PolkitQt1::Agent::Session *session;
        session = new PolkitQt1::Agent::Session(i, cookie, result);
        Q_ASSERT(session);
        m_sessionIdentity[session] = i;
        connect(session, &PolkitQt1::Agent::Session::request, this, &PolKitAgentListener::request);
        connect(session, &PolkitQt1::Agent::Session::completed, this, &PolKitAgentListener::completed);
        session->initiate();
    }
}

void PolKitAgentListener::request(const QString &request, bool echo)
{
    Q_UNUSED(request);
    Q_UNUSED(echo);
    qDebug() << "POLKIT CALLED" << 1;

    PolkitQt1::Agent::Session *session = qobject_cast<PolkitQt1::Agent::Session *>(sender());
    Q_ASSERT(session);
    Q_ASSERT(m_dialog);

    PolkitQt1::Identity identity = m_sessionIdentity[session];

    connect(m_dialog, &Dialog::finished, this, [=] {
        if (m_dialog->identity() == m_sessionIdentity[session].toString()
                && !m_dialog->password().isEmpty()) {
            session->setResponse(m_dialog->password());
        } else {
            session->cancel();
        }
    });

    emit this->authenticationRequest(m_dialog);
}

void PolKitAgentListener::completed(bool gainedAuthorization)
{
    Q_UNUSED(gainedAuthorization);
    qDebug() << "POLKIT CALLED" << 2;

    PolkitQt1::Agent::Session *session = qobject_cast<PolkitQt1::Agent::Session *>(sender());
    Q_ASSERT(session);
    Q_ASSERT(m_dialog);

    if (m_dialog->identity() == m_sessionIdentity[session].toString()) {
        session->result()->setCompleted();

        m_dialog->deleteLater();
        m_dialog = nullptr;
    }

    session->deleteLater();
}
