/*
    SPDX-FileCopyrightText: 2018 Nicolas Fella <nicolas.fella@gmx.de>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "canberracontext.h"

namespace QPulseAudio
{
CanberraContext *CanberraContext::s_context = nullptr;

CanberraContext *CanberraContext::instance()
{
    if (!s_context) {
        s_context = new CanberraContext;
    }
    return s_context;
}

CanberraContext::CanberraContext(QObject *parent)
    : QObject(parent)
{
    ca_context_create(&m_canberra);
}

CanberraContext::~CanberraContext()
{
    if (m_canberra) {
        ca_context_destroy(m_canberra);
    }
}

ca_context *CanberraContext::canberra()
{
    return m_canberra;
}

void CanberraContext::ref()
{
    ++m_references;
}

void CanberraContext::unref()
{
    if (--m_references == 0) {
        delete this;
        s_context = nullptr;
    }
}
}
