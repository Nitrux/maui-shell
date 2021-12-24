/*
    SPDX-FileCopyrightText: 2016 David Rosca <nowrep@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef VOLUMEFEEDBACK_H
#define VOLUMEFEEDBACK_H

#include <QObject>

#include <canberra.h>

class VolumeFeedback : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool valid READ isValid CONSTANT)

public:
    explicit VolumeFeedback(QObject *parent = nullptr);
    ~VolumeFeedback() override;

    bool isValid() const;

public Q_SLOTS:
    void play(quint32 sinkIndex);
};

#endif // VOLUMEFEEDBACK_H
