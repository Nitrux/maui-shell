/*
    SPDX-FileCopyrightText: 2021 Kai Uwe Broulik <kde@broulik.de>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once

#include <QObject>
#include <QPointer>
#include <QQmlParserStatus>

class QAbstractItemModel;
class QMenu;
class QQuickItem;

namespace QPulseAudio
{
class CardModel;
class PulseObject;
}

class ListItemMenu : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)

    Q_PROPERTY(ItemType itemType READ itemType WRITE setItemType NOTIFY itemTypeChanged)

    Q_PROPERTY(QPulseAudio::PulseObject *pulseObject READ pulseObject WRITE setPulseObject NOTIFY pulseObjectChanged)

    Q_PROPERTY(QAbstractItemModel *sourceModel READ sourceModel WRITE setSourceModel NOTIFY sourceModelChanged)

    Q_PROPERTY(QPulseAudio::CardModel *cardModel READ cardModel WRITE setCardModel NOTIFY cardModelChanged)

    Q_PROPERTY(bool visible READ isVisible NOTIFY visibleChanged)

    Q_PROPERTY(bool hasContent READ hasContent NOTIFY hasContentChanged)

    Q_PROPERTY(QQuickItem *visualParent READ visualParent WRITE setVisualParent NOTIFY visualParentChanged)

public:
    explicit ListItemMenu(QObject *parent = nullptr);
    ~ListItemMenu() override;

    enum ItemType {
        None,
        Sink,
        SinkInput,
        Source,
        SourceOutput,
    };
    Q_ENUM(ItemType)

    ItemType itemType() const;
    void setItemType(ItemType itemType);
    Q_SIGNAL void itemTypeChanged();

    QPulseAudio::PulseObject *pulseObject() const;
    void setPulseObject(QPulseAudio::PulseObject *pulseObject);
    Q_SIGNAL void pulseObjectChanged();

    QAbstractItemModel *sourceModel() const;
    void setSourceModel(QAbstractItemModel *sourceModel);
    Q_SIGNAL void sourceModelChanged();

    QPulseAudio::CardModel *cardModel() const;
    void setCardModel(QPulseAudio::CardModel *cardModel);
    Q_SIGNAL void cardModelChanged();

    bool isVisible() const;
    Q_SIGNAL void visibleChanged();

    bool hasContent() const;
    Q_SIGNAL void hasContentChanged();

    QQuickItem *visualParent() const;
    void setVisualParent(QQuickItem *visualParent);
    Q_SIGNAL void visualParentChanged();

    void classBegin() override;
    void componentComplete() override;

    Q_INVOKABLE void open(int x, int y);
    Q_INVOKABLE void openRelative();

private:
    void setVisible(bool visible);

    void update();
    bool checkHasContent();
    QMenu *createMenu();

    bool m_complete = false;
    bool m_visible = false;
    bool m_hasContent = false;
    QPointer<QQuickItem> m_visualParent;

    ItemType m_itemType = None;
    QPointer<QPulseAudio::PulseObject> m_pulseObject;
    QPointer<QAbstractItemModel> m_sourceModel;
    QPointer<QPulseAudio::CardModel> m_cardModel;
};
