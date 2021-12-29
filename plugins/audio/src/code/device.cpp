/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "device.h"

QPulseAudio::Device::State QPulseAudio::Device::state() const
{
    return m_state;
}

QString QPulseAudio::Device::name() const
{
    return m_name;
}

QString QPulseAudio::Device::description() const
{
    return m_description;
}

QString QPulseAudio::Device::formFactor() const
{
    return m_formFactor;
}

quint32 QPulseAudio::Device::cardIndex() const
{
    return m_cardIndex;
}

QList<QObject *> QPulseAudio::Device::ports() const
{
    return m_ports;
}

quint32 QPulseAudio::Device::activePortIndex() const
{
    return m_activePortIndex;
}

bool QPulseAudio::Device::isVirtualDevice() const
{
    return m_virtualDevice;
}

QPulseAudio::Device::Device(QObject *parent)
    : VolumeObject(parent)
{
}

QPulseAudio::Device::State QPulseAudio::Device::stateFromPaState(int value) const
{
    switch (value) {
    case -1: // PA_X_INVALID_STATE
        return InvalidState;
    case 0: // PA_X_RUNNING
        return RunningState;
    case 1: // PA_X_IDLE
        return IdleState;
    case 2: // PA_X_SUSPENDED
        return SuspendedState;
    default:
        return UnknownState;
    }
}
