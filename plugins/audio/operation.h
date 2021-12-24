/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef OPERATION_H
#define OPERATION_H

#include <pulse/operation.h>

namespace QPulseAudio
{
/**
 * @brief The PAOperation class
 * Helps with management of pa_operations. pa_operations need to be expicitly
 * unref'd after use, so this class is essentially a fancy scoping helper where
 * destruction of an instance would also unref the held operation (if there is
 * one).
 */
class PAOperation
{
public:
    /**
     * @brief PAOperation
     * @param operation operation to manage the scope of
     */
    explicit PAOperation(pa_operation *operation = nullptr);
    ~PAOperation();

    PAOperation &operator=(pa_operation *operation);

    /**
     * @brief operator !
     * @return whether or not there is an operation pointer
     */
    bool operator!();

    /**
     * @brief operator bool representing whether there is an operation
     */
    operator bool();

    /**
     * @brief operator *
     * @return pointer to internal pa_operation object
     */
    pa_operation *&operator*();

private:
    pa_operation *m_operation;
};

} // QPulseAudio

#endif // OPERATION_H
