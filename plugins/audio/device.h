/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef DEVICE_H
#define DEVICE_H

#include <QString>

#include <pulse/volume.h>

#include "port.h"
#include "pulseobject.h"
#include "volumeobject.h"

namespace QPulseAudio
{
class Device : public VolumeObject
{
    Q_OBJECT
    Q_PROPERTY(State state READ state NOTIFY stateChanged)
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)
    Q_PROPERTY(QString formFactor READ formFactor NOTIFY formFactorChanged)
    Q_PROPERTY(quint32 cardIndex READ cardIndex NOTIFY cardIndexChanged)
    Q_PROPERTY(QList<QObject *> ports READ ports NOTIFY portsChanged)
    Q_PROPERTY(quint32 activePortIndex READ activePortIndex WRITE setActivePortIndex NOTIFY activePortIndexChanged)
    Q_PROPERTY(bool default READ isDefault WRITE setDefault NOTIFY defaultChanged)
    Q_PROPERTY(bool virtualDevice READ isVirtualDevice NOTIFY virtualDeviceChanged)
public:
    enum State {
        InvalidState = 0,
        RunningState,
        IdleState,
        SuspendedState,
        UnknownState,
    };
    Q_ENUMS(State);

    ~Device() override
    {
    }

    template<typename PAInfo>
    void updateDevice(const PAInfo *info)
    {
        updateVolumeObject(info);

        if (m_name != info->name) {
            m_name = info->name;
            Q_EMIT nameChanged();
        }
        if (m_description != info->description) {
            m_description = info->description;
            Q_EMIT descriptionChanged();
        }
        const char *form_factor = pa_proplist_gets(info->proplist, PA_PROP_DEVICE_FORM_FACTOR);
        if (form_factor) {
            QString formFactor = QString::fromUtf8(form_factor);
            if (m_formFactor != formFactor) {
                m_formFactor = formFactor;
                Q_EMIT formFactorChanged();
            }
        }

        if (m_cardIndex != info->card) {
            m_cardIndex = info->card;
            Q_EMIT cardIndexChanged();
        }

        const quint32 oldActivePortIndex = m_activePortIndex;
        bool portsHaveChanged = false;
        int i = 0;
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
            if (info->active_port == *ports) {
                m_activePortIndex = i;
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
        if (portsHaveChanged || m_activePortIndex != oldActivePortIndex) {
            Q_EMIT activePortIndexChanged();
        }

        State infoState = stateFromPaState(info->state);
        if (infoState != m_state) {
            m_state = infoState;
            Q_EMIT stateChanged();
        }

        const bool isVirtual = !(info->flags & 4); // PA_X_HARDWARE
        if (m_virtualDevice != isVirtual) {
            m_virtualDevice = isVirtual;
            Q_EMIT virtualDeviceChanged();
        }
    }

    State state() const;
    QString name() const;
    QString description() const;
    QString formFactor() const;
    quint32 cardIndex() const;
    QList<QObject *> ports() const;
    quint32 activePortIndex() const;
    virtual void setActivePortIndex(quint32 port_index) = 0;
    virtual bool isDefault() const = 0;
    virtual void setDefault(bool enable) = 0;
    bool isVirtualDevice() const;

    virtual Q_INVOKABLE void switchStreams() = 0;

Q_SIGNALS:
    void stateChanged();
    void nameChanged();
    void descriptionChanged();
    void formFactorChanged();
    void cardIndexChanged();
    void portsChanged();
    void activePortIndexChanged();
    void defaultChanged();
    void virtualDeviceChanged();

protected:
    explicit Device(QObject *parent);

private:
    State stateFromPaState(int value) const;

    QString m_name;
    QString m_description;
    QString m_formFactor;
    quint32 m_cardIndex = -1;
    QList<QObject *> m_ports;
    quint32 m_activePortIndex = -1;
    State m_state = UnknownState;
    bool m_virtualDevice = false;
};

} // QPulseAudio

#endif // DEVICE_H
