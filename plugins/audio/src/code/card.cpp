/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "card.h"

#include "debug.h"

#include "context.h"

namespace QPulseAudio
{
Card::Card(QObject *parent)
    : PulseObject(parent)
{
}

void Card::update(const pa_card_info *info)
{
    updatePulseObject(info);

    QString infoName = QString::fromUtf8(info->name);
    if (m_name != infoName) {
        m_name = infoName;
        Q_EMIT nameChanged();
    }

    const quint32 oldActiveProfileIndex = m_activeProfileIndex;
    bool profilesHaveChanged = false;
    int i = 0;
    for (auto **it = info->profiles2; it && *it != nullptr; ++it) {
        if (i < m_profiles.count()) {
            Profile *profile = static_cast<Profile *>(m_profiles.at(i));
            profilesHaveChanged |= profile->setInfo(*it);
        } else {
            Profile *profile = new Profile(this);
            profile->setInfo(*it);
            m_profiles.append(profile);
            profilesHaveChanged = true;
        }
        if (info->active_profile2 == *it) {
            m_activeProfileIndex = i;
        }
        ++i;
    }

    while (m_profiles.count() > i) {
        delete m_profiles.takeLast();
        profilesHaveChanged = true;
    }

    if (profilesHaveChanged) {
        Q_EMIT profilesChanged();
    }
    if (profilesHaveChanged || m_activeProfileIndex != oldActiveProfileIndex) {
        Q_EMIT activeProfileIndexChanged();
    }

    bool portsHaveChanged = false;
    i = 0;
    for (auto **ports = info->ports; ports && *ports != nullptr; ++ports) {
        if (i < m_ports.count()) {
            Port *port = static_cast<Port *>(m_ports.at(i));
            portsHaveChanged |= port->setInfo(*ports);
        } else {
            Port *port = new Port(this);
            port->setInfo(*ports);
            m_ports.append(port);
            portsHaveChanged = true;
        }
        ++i;
    }

    while (m_ports.count() > i) {
        delete m_ports.takeLast();
        portsHaveChanged = true;
    }

    if (portsHaveChanged) {
        Q_EMIT portsChanged();
    }
}

QString Card::name() const
{
    return m_name;
}

QList<QObject *> Card::profiles() const
{
    return m_profiles;
}

quint32 Card::activeProfileIndex() const
{
    return m_activeProfileIndex;
}

void Card::setActiveProfileIndex(quint32 profileIndex)
{
    const Profile *profile = qobject_cast<Profile *>(profiles().at(profileIndex));
    context()->setCardProfile(index(), profile->name());
}

QList<QObject *> Card::ports() const
{
    return m_ports;
}

} // QPulseAudio
