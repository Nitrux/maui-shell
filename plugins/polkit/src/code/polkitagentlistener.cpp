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

}

void PolKitAgentListener::initiateAuthentication(const QString &actionId,
                                                 const QString &message,
                                                 const QString &iconName,
                                                 const PolkitQt1::Details &details,
                                                 const QString &cookie,
                                                 const PolkitQt1::Identity::List &identities,
                                                 PolkitQt1::Agent::AsyncResult *result)
{
    Q_UNUSED(iconName);
    Q_UNUSED(details);

    if (identities.isEmpty()) {
        qWarning() << "Polkit authentication requested without any identity";
        if (result)
            result->setCompleted();
        return;
    }

    if (m_dialog) {
        m_dialog->deleteLater();
        m_dialog = nullptr;
    }
    m_pendingSession = nullptr;

    m_dialog = new Dialog(actionId, message, cookie, identities.first().toString(), result);
    connect(m_dialog, &Dialog::finished, this, &PolKitAgentListener::dialogFinished);

    for (const PolkitQt1::Identity &i : identities) {
        auto *session = new PolkitQt1::Agent::Session(i, cookie, result);
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

    auto *session = qobject_cast<PolkitQt1::Agent::Session *>(sender());
    if (!session)
        return;

    if (!m_dialog) {
        session->cancel();
        return;
    }

    const auto identity = m_sessionIdentity.value(session);
    if (identity.toString() != m_dialog->identity()) {
        session->cancel();
        return;
    }

    m_pendingSession = session;
    Q_EMIT this->authenticationRequest(m_dialog);
}

void PolKitAgentListener::dialogFinished()
{
    auto *session = m_pendingSession.data();
    m_pendingSession = nullptr;

    QPointer<Dialog> dialog = m_dialog;
    m_dialog = nullptr;

    if (!dialog)
        return;

    if (session) {
        const bool accepted = dialog->identity() == m_sessionIdentity.value(session).toString()
                && !dialog->password().isEmpty();

        if (accepted)
            session->setResponse(dialog->password());
        else
            session->cancel();
    }

    dialog->deleteLater();
}

void PolKitAgentListener::completed(bool gainedAuthorization)
{
    Q_UNUSED(gainedAuthorization);

    auto *session = qobject_cast<PolkitQt1::Agent::Session *>(sender());
    if (!session)
        return;

    const auto identity = m_sessionIdentity.take(session);

    if (session->result())
        session->result()->setCompleted();

    if (m_pendingSession == session)
        m_pendingSession = nullptr;

    if (m_dialog && m_dialog->identity() == identity.toString()) {
        m_dialog->deleteLater();
        m_dialog = nullptr;
    }

    session->deleteLater();
}
