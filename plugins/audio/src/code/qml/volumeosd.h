/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef VOLUMEOSD_H
#define VOLUMEOSD_H

#include <QObject>

class VolumeOSD : public QObject
{
    Q_OBJECT
public:
    explicit VolumeOSD(QObject *parent = nullptr);

public Q_SLOTS:
    void show(int percent, int maximumPercent = 100);
    void showMicrophone(int percent);
    void showText(const QString &iconName, const QString &text);
};

#endif // VOLUMEOSD_H
