/*
    SPDX-FileCopyrightText: 2018 Nicolas Fella <nicolas.fella@gmx.de>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/
#pragma once

#include <QObject>
#include <canberra.h>

namespace QPulseAudio
{
class CanberraContext : public QObject
{
    Q_OBJECT

public:
    explicit CanberraContext(QObject *parent = nullptr);
    virtual ~CanberraContext();

    static CanberraContext *instance();

    ca_context *canberra();

    void ref();
    void unref();

private:
    ca_context *m_canberra = nullptr;
    int m_references = 0;

    static CanberraContext *s_context;
};

}
