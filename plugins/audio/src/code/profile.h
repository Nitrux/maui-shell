/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PROFILE_H
#define PROFILE_H

#include <QObject>
#include <QString>

namespace QPulseAudio
{
class Profile : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)
    Q_PROPERTY(quint32 priority READ priority NOTIFY priorityChanged)
    Q_PROPERTY(Availability availability READ availability NOTIFY availabilityChanged)
public:
    enum Availability {
        Unknown,
        Available,
        Unavailable,
    };
    Q_ENUM(Availability)

    explicit Profile(QObject *parent);
    ~Profile() override;

    template<typename PAInfo>
    bool setInfo(const PAInfo *info)
    {
        return setCommonInfo(info, info->available ? Available : Unavailable);
    }

    QString name() const;
    QString description() const;
    quint32 priority() const;
    Availability availability() const;

Q_SIGNALS:
    void nameChanged();
    void descriptionChanged();
    void priorityChanged();
    void availabilityChanged();

protected:
    template<typename PAInfo>
    bool setCommonInfo(const PAInfo *info, Availability newAvailability)
    {
        bool changed = false;

        // Description is optional. Name not so much as we need some ID.
        Q_ASSERT(info->name);
        QString infoName = QString::fromUtf8(info->name);
        if (m_name != infoName) {
            m_name = infoName;
            Q_EMIT nameChanged();
            changed = true;
        }
        if (info->description) {
            QString infoDescription = QString::fromUtf8(info->description);
            if (m_description != infoDescription) {
                m_description = infoDescription;
                Q_EMIT descriptionChanged();
                changed = true;
            }
        }
        if (m_priority != info->priority) {
            m_priority = info->priority;
            Q_EMIT priorityChanged();
            changed = true;
        }

        if (m_availability != newAvailability) {
            m_availability = newAvailability;
            Q_EMIT availabilityChanged();
            changed = true;
        }

        return changed;
    }

private:
    QString m_name;
    QString m_description;
    quint32 m_priority;
    Availability m_availability;
};

} // QPulseAudio

#endif // PROFILE_H
