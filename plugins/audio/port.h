/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PORT_H
#define PORT_H

#include "profile.h"

#include <pulse/def.h>

namespace QPulseAudio
{
class Port : public Profile
{
    Q_OBJECT

public:
    explicit Port(QObject *parent);
    ~Port() override;

    template<typename PAInfo>
    bool setInfo(const PAInfo *info)
    {
        Availability newAvailability;
        switch (info->available) {
        case PA_PORT_AVAILABLE_NO:
            newAvailability = Unavailable;
            break;
        case PA_PORT_AVAILABLE_YES:
            newAvailability = Available;
            break;
        default:
            newAvailability = Unknown;
        }
        return setCommonInfo(info, newAvailability);
    }
};

} // QPulseAudio

#endif // PORT_H
