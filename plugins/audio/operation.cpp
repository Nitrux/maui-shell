/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "operation.h"

namespace QPulseAudio
{
PAOperation::PAOperation(pa_operation *operation)
    : m_operation(operation)
{
}

PAOperation::~PAOperation()
{
    if (m_operation) {
        pa_operation_unref(m_operation);
    }
}

PAOperation &PAOperation::operator=(pa_operation *operation)
{
    m_operation = operation;
    return *this;
}

bool PAOperation::operator!()
{
    return !m_operation;
}

pa_operation *&PAOperation::operator*()
{
    return m_operation;
}

PAOperation::operator bool()
{
    return m_operation;
}

} // QPulseAudio
