/*
    SPDX-FileCopyrightText: 2021 Kai Uwe Broulik <kde@broulik.de>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "listitemmenu.h"

#include <QAbstractItemModel>
#include <QMenu>
#include <QQuickItem>
#include <QQuickWindow>
#include <QWindow>

#include "card.h"
#include "debug.h"
#include "device.h"
#include "port.h"
#include "pulseaudio.h"
#include "pulseobject.h"
#include "stream.h"

using namespace QPulseAudio;

static const auto s_offProfile = QLatin1String("off");

ListItemMenu::ListItemMenu(QObject *parent)
    : QObject(parent)
{
}

ListItemMenu::~ListItemMenu() = default;

void ListItemMenu::classBegin()
{
}

void ListItemMenu::componentComplete()
{
    m_complete = true;
    update();
}

ListItemMenu::ItemType ListItemMenu::itemType() const
{
    return m_itemType;
}

void ListItemMenu::setItemType(ItemType itemType)
{
    if (m_itemType != itemType) {
        m_itemType = itemType;
        update();
        Q_EMIT itemTypeChanged();
    }
}

QPulseAudio::PulseObject *ListItemMenu::pulseObject() const
{
    return m_pulseObject.data();
}

void ListItemMenu::setPulseObject(QPulseAudio::PulseObject *pulseObject)
{
    if (m_pulseObject.data() != pulseObject) {
        // TODO is Qt clever enough to catch the disconnect from base class?
        if (m_pulseObject) {
            disconnect(m_pulseObject, nullptr, this, nullptr);
        }

        m_pulseObject = pulseObject;

        if (auto *device = qobject_cast<QPulseAudio::Device *>(m_pulseObject.data())) {
            connect(device, &Device::activePortIndexChanged, this, &ListItemMenu::update);
            connect(device, &Device::portsChanged, this, &ListItemMenu::update);
        }

        update();
        Q_EMIT pulseObjectChanged();
    }
}

QAbstractItemModel *ListItemMenu::sourceModel() const
{
    return m_sourceModel.data();
}

void ListItemMenu::setSourceModel(QAbstractItemModel *sourceModel)
{
    if (m_sourceModel.data() == sourceModel) {
        return;
    }

    if (m_sourceModel) {
        disconnect(m_sourceModel, nullptr, this, nullptr);
    }

    m_sourceModel = sourceModel;

    if (m_sourceModel) {
        connect(m_sourceModel, &QAbstractItemModel::rowsInserted, this, &ListItemMenu::update);
        connect(m_sourceModel, &QAbstractItemModel::rowsRemoved, this, &ListItemMenu::update);
        connect(m_sourceModel, &QAbstractItemModel::modelReset, this, &ListItemMenu::update);
    }

    update();
    Q_EMIT sourceModelChanged();
}

QPulseAudio::CardModel *ListItemMenu::cardModel() const
{
    return m_cardModel.data();
}

void ListItemMenu::setCardModel(QPulseAudio::CardModel *cardModel)
{
    if (m_cardModel.data() == cardModel) {
        return;
    }

    if (m_cardModel) {
        disconnect(m_cardModel, nullptr, this, nullptr);
    }
    m_cardModel = cardModel;

    if (m_cardModel) {
        const int profilesRole = m_cardModel->role("Profiles");
        Q_ASSERT(profilesRole > -1);

        connect(m_cardModel, &CardModel::dataChanged, this, [this, profilesRole](const QModelIndex &, const QModelIndex &, const QVector<int> &roles) {
            if (roles.isEmpty() || roles.contains(profilesRole)) {
                update();
            }
        });
    }

    update();
    Q_EMIT cardModelChanged();
}

bool ListItemMenu::isVisible() const
{
    return m_visible;
}

void ListItemMenu::setVisible(bool visible)
{
    if (m_visible != visible) {
        m_visible = visible;
        Q_EMIT visibleChanged();
    }
}

bool ListItemMenu::hasContent() const
{
    return m_hasContent;
}

QQuickItem *ListItemMenu::visualParent() const
{
    return m_visualParent.data();
}

void ListItemMenu::setVisualParent(QQuickItem *visualParent)
{
    if (m_visualParent.data() != visualParent) {
        m_visualParent = visualParent;
        Q_EMIT visualParentChanged();
    }
}

bool ListItemMenu::checkHasContent()
{
    // If there are at least two sink/source devices to choose from.
    if (m_sourceModel && m_sourceModel->rowCount() > 1) {
        return true;
    }

    auto *device = qobject_cast<QPulseAudio::Device *>(m_pulseObject.data());

    if (device) {
        const auto ports = device->ports();
        if (ports.length() > 1) {
            // In case an unavailable port is active.
            if (device->activePortIndex() != static_cast<quint32>(-1)) {
                auto *activePort = static_cast<Port *>(ports.at(device->activePortIndex()));
                if (activePort->availability() == Port::Unavailable) {
                    return true;
                }
            }

            // If there are at least two available ports.
            int availablePorts = 0;
            for (auto *portObject : ports) {
                auto *port = static_cast<Port *>(portObject);
                if (port->availability() == Port::Unavailable) {
                    continue;
                }

                if (++availablePorts == 2) {
                    return true;
                }
            }
        }

        if (m_cardModel) {
            const int cardModelPulseObjectRole = m_cardModel->role("PulseObject");
            Q_ASSERT(cardModelPulseObjectRole != -1);

            for (int i = 0; i < m_cardModel->rowCount(); ++i) {
                const QModelIndex cardIdx = m_cardModel->index(i, 0);
                Card *card = qobject_cast<Card *>(cardIdx.data(cardModelPulseObjectRole).value<QObject *>());

                if (card->index() == device->cardIndex()) {
                    // If there are at least two available profiles on the corresponding card.
                    const auto profiles = card->profiles();
                    int availableProfiles = 0;
                    for (auto *profileObject : profiles) {
                        auto *profile = static_cast<Profile *>(profileObject);
                        if (profile->availability() == Profile::Unavailable) {
                            continue;
                        }

                        if (profile->name() == s_offProfile) {
                            continue;
                        }

                        // TODO should we also check "if current profile is unavailable" like with ports?
                        if (++availableProfiles == 2) {
                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}

void ListItemMenu::update()
{
    if (!m_complete) {
        return;
    }

    const bool hasContent = checkHasContent();
    if (m_hasContent != hasContent) {
        m_hasContent = hasContent;
        Q_EMIT hasContentChanged();
    }
}

void ListItemMenu::open(int x, int y)
{
    auto *menu = createMenu();
    if (!menu) {
        return;
    }

    const QPoint pos = m_visualParent->mapToGlobal(QPointF(x, y)).toPoint();

    menu->popup(pos);
    setVisible(true);
}

// to the bottom left of visualParent
void ListItemMenu::openRelative()
{
    auto *menu = createMenu();
    if (!menu) {
        return;
    }

    menu->adjustSize();

    QPoint pos = m_visualParent->mapToGlobal(QPointF(m_visualParent->width(), m_visualParent->height())).toPoint();
    pos.rx() -= menu->width();

    // TODO do we still need this ungrab mouse hack?
    menu->popup(pos);
    setVisible(true);
}

static int getModelRole(QObject *model, const QByteArray &name)
{
    // Can either be an AbstractModel, then it's easy
    if (auto *abstractModel = qobject_cast<AbstractModel *>(model)) {
        return abstractModel->role(name);
    }

    // or that PulseObjectFilterModel from QML where everything is a QVariant...
    QVariant roleVariant;
    bool ok = QMetaObject::invokeMethod(model, "role", Q_RETURN_ARG(QVariant, roleVariant), Q_ARG(QVariant, QVariant(name)));
    if (!ok) {
        qCCritical(PLASMAPA) << "Failed to invoke 'role' on" << model;
        return -1;
    }

    int role = roleVariant.toInt(&ok);
    if (!ok) {
        qCCritical(PLASMAPA) << "Return value from 'role' is bogus" << roleVariant;
        return -1;
    }

    return role;
}

QMenu *ListItemMenu::createMenu()
{
    if (m_visible) {
        return nullptr;
    }

    if (!m_visualParent || !m_visualParent->window()) {
        qCWarning(PLASMAPA) << "Cannot prepare menu without visualParent or a window";
        return nullptr;
    }

    QMenu *menu = new QMenu();
    menu->setAttribute(Qt::WA_DeleteOnClose);

    connect(menu, &QMenu::aboutToHide, this, [this] {
        setVisible(false);
    });

    if (auto *device = qobject_cast<QPulseAudio::Device *>(m_pulseObject.data())) {
        // Switch all streams of the relevant kind to this device
        if (m_sourceModel->rowCount() > 1) {
            QAction *switchStreamsAction = nullptr;
            if (m_itemType == Sink) {
                switchStreamsAction = menu->addAction(
                    QIcon::fromTheme(QStringLiteral("audio-on"),
                                     QIcon::fromTheme(QStringLiteral("audio-ready"), QIcon::fromTheme(QStringLiteral("audio-speakers-symbolic")))),
                    tr("Play all audio via this device"));
            } else if (m_itemType == Source) {
                switchStreamsAction = menu->addAction(
                    QIcon::fromTheme(QStringLiteral("mic-on"),
                                     QIcon::fromTheme(QStringLiteral("mic-ready"), QIcon::fromTheme(QStringLiteral("audio-input-microphone-symbolic")))),
                    tr("Record all audio via this device"));
            }

            if (switchStreamsAction) {
                connect(switchStreamsAction, &QAction::triggered, device, &Device::switchStreams);
            }
        }

        // Ports
        const auto ports = device->ports();
        bool activePortUnavailable = false;
        if (device->activePortIndex() != static_cast<quint32>(-1)) {
            auto *activePort = static_cast<Port *>(ports.at(device->activePortIndex()));
            activePortUnavailable = activePort->availability() == Port::Unavailable;
        }

        QMap<int, Port *> availablePorts;
        for (int i = 0; i < ports.count(); ++i) {
            auto *port = static_cast<Port *>(ports.at(i));

            // If an unavailable port is active, show all the ports,
            // otherwise show only the available ones
            if (activePortUnavailable || port->availability() != Port::Unavailable) {
                availablePorts.insert(i, port);
            }
        }

        if (availablePorts.count() > 1) {
            menu->addSection(tr("Heading for a list of ports of a device (for example built-in laptop speakers or a plug for headphones)", "Ports"));

            auto *portGroup = new QActionGroup(menu);

            for (auto it = availablePorts.constBegin(), end = availablePorts.constEnd(); it != end; ++it) {
                const int i = it.key();
                Port *port = it.value();

                QAction *item = nullptr;

                if (port->availability() == Port::Unavailable) {
                    if (port->name() == QLatin1String("analog-output-speaker") || port->name() == QLatin1String("analog-input-microphone-internal")) {
                        item = menu->addAction(tr("%1 (unavailable)").arg(port->description()));
                    } else {
                        item = menu->addAction(tr("%1 (unplugged)").arg(port->description()));
                    }
                } else {
                    item = menu->addAction(port->description());
                }

                item->setCheckable(true);
                item->setChecked(static_cast<quint32>(i) == device->activePortIndex());
                connect(item, &QAction::triggered, device, [device, i] {
                    device->setActivePortIndex(i);
                });

                portGroup->addAction(item);
            }
        }

        // Submenu with profiles
        if (m_cardModel) {
            const int cardModelPulseObjectRole = m_cardModel->role("PulseObject");
            Q_ASSERT(cardModelPulseObjectRole != -1);

            Card *card = nullptr;
            for (int i = 0; i < m_cardModel->rowCount(); ++i) {
                const QModelIndex cardIdx = m_cardModel->index(i, 0);
                Card *candidateCard = qobject_cast<Card *>(cardIdx.data(cardModelPulseObjectRole).value<QObject *>());

                if (candidateCard && candidateCard->index() == device->cardIndex()) {
                    card = candidateCard;
                    break;
                }
            }

            if (card) {
                QMap<int, Profile *> availableProfiles;

                const auto profiles = card->profiles();
                for (int i = 0; i < profiles.count(); ++i) {
                    auto *profile = static_cast<Profile *>(profiles.at(i));

                    // TODO should we also check "if current profile is unavailable" like with ports?
                    if (profile->availability() == Profile::Unavailable) {
                        continue;
                    }

                    // Don't let user easily remove a device with no obvious way to get it back
                    // Only let that be done from the KCM where one can just flip the ComboBox back.
                    if (profile->name() == s_offProfile) {
                        continue;
                    }

                    availableProfiles.insert(i, profile);
                }

                if (availableProfiles.count() > 1) {
                    // If there's too many profiles, put them in a submenu, unless the menu is empty, otherwise as a section
                    QMenu *profilesMenu = menu;
                    const QString title = tr("Heading for a list of device profiles (5.1 surround sound, stereo, speakers only, ...)", "Profiles");
                    // "10" is catered around laptop speakers (internal, stereo, duplex) plus one HDMI port (stereo, surround 5.1, 7.1, in and out, etc)
                    if (availableProfiles.count() > 10 && !menu->actions().isEmpty()) {
                        profilesMenu = menu->addMenu(title);
                    } else {
                        menu->addSection(title);
                    }

                    QActionGroup *profileGroup = new QActionGroup(profilesMenu);
                    for (auto it = availableProfiles.constBegin(), end = availableProfiles.constEnd(); it != end; ++it) {
                        const int i = it.key();
                        Profile *profile = it.value();

                        auto *profileAction = profilesMenu->addAction(profile->description());
                        profileAction->setCheckable(true);
                        profileAction->setChecked(static_cast<quint32>(i) == card->activeProfileIndex());
                        connect(profileAction, &QAction::triggered, card, [card, i] {
                            card->setActiveProfileIndex(i);
                        });

                        profileGroup->addAction(profileAction);
                    }
                }
            } else {
                qCWarning(PLASMAPA) << "Failed to find card at" << device->cardIndex() << "for" << device->description() << device->index();
            }
        }
    }

    // Choose output / input device
    auto *stream = qobject_cast<QPulseAudio::Stream *>(m_pulseObject.data());
    if (stream && m_sourceModel && m_sourceModel->rowCount() > 1) {
        if (m_itemType == SinkInput || m_itemType == SourceOutput) {
            if (m_itemType == SinkInput) {
                menu->addSection(tr("Heading for a list of possible output devices (speakers, headphones, ...) to choose", "Play audio using"));
            } else {
                menu->addSection(tr("Heading for a list of possible input devices (built-in microphone, headset, ...) to choose", "Record audio using"));
            }

            const int indexRole = getModelRole(m_sourceModel, "Index");
            Q_ASSERT(indexRole > -1);
            const int descriptionRole = getModelRole(m_sourceModel, "Description");
            Q_ASSERT(descriptionRole > -1);

            auto *deviceGroup = new QActionGroup(menu);

            for (int i = 0; i < m_sourceModel->rowCount(); ++i) {
                const QModelIndex idx = m_sourceModel->index(i, 0);
                const auto index = idx.data(indexRole).toUInt();

                auto *item = menu->addAction(idx.data(descriptionRole).toString());
                item->setCheckable(true);
                item->setChecked(index == stream->deviceIndex());
                connect(item, &QAction::triggered, stream, [stream, index] {
                    stream->setDeviceIndex(index);
                });

                deviceGroup->addAction(item);
            }
        }
    }

    if (menu->isEmpty()) {
        delete menu;
        return nullptr;
    }

    menu->winId();
    menu->windowHandle()->setTransientParent(m_visualParent->window());

    return menu;
}
