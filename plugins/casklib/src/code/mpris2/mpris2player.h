// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QtCore/QObject>
#include <QtCore/QDateTime>
#include <QtCore/QLoggingCategory>
#include <QtDBus/QDBusMessage>
#include <QtQml/QQmlPropertyMap>
#include <QDBusObjectPath>

class QDBusPendingCallWatcher;
class OrgFreedesktopDBusPropertiesInterface;
class OrgMprisMediaPlayer2Interface;
class OrgMprisMediaPlayer2PlayerInterface;

Q_DECLARE_LOGGING_CATEGORY(MPRIS2_PLAYER)

typedef QMap<QString, QVariant> Metadata;

class Mpris2Player : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString identity READ identity NOTIFY identityChanged)
    Q_PROPERTY(QString iconName READ iconName NOTIFY iconNameChanged)
    Q_PROPERTY(Capabilities capabilities READ capabilities WRITE setCapabilities NOTIFY capabilitiesChanged)
    Q_PROPERTY(QQmlPropertyMap *metadata READ metadata NOTIFY metadataChanged)
    Q_PROPERTY(qlonglong position READ position NOTIFY positionChanged)
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)
    Q_PROPERTY(bool fullScreen READ isFullScreen NOTIFY fullScreenChanged)
    Q_PROPERTY(qreal rate READ rate NOTIFY rateChanged)
    Q_PROPERTY(qreal minimumRate READ minimumRate NOTIFY minimumRateChanged)
    Q_PROPERTY(qreal maximumRate READ maximumRate NOTIFY maximumRateChanged)
    Q_PROPERTY(QString serviceName READ serviceName NOTIFY serviceNameChanged)

public:
    enum Capability {
        NoCapabilities = 0,
        CanQuit = 1 << 0,
        CanRaise = 1 << 1,
        CanSetFullscreen = 1 << 2,
        CanControl = 1 << 3,
        CanPlay = 1 << 4,
        CanPause = 1 << 5,
        CanStop = 1 << 6,
        CanSeek = 1 << 7,
        CanGoNext = 1 << 8,
        CanGoPrevious = 1 << 9
    };
    Q_ENUM(Capability)
    Q_DECLARE_FLAGS(Capabilities, Capability)
    Q_FLAGS(Capabilities)

    Mpris2Player(const QString &service, QObject *parent = 0);
    QDBusObjectPath trackId() const;

	QString serviceName() const;

    QString identity() const;
    QString iconName() const;

    Capabilities capabilities() const;

    QQmlPropertyMap *metadata() const;

    QString status() const;

    bool isFullScreen() const;

    qlonglong position() const;

    qreal rate() const;
    qreal minimumRate() const;
    qreal maximumRate() const;

    qreal volume() const;

Q_SIGNALS:
    void identityChanged();
    void iconNameChanged();
    void capabilitiesChanged();
    void metadataChanged();
    void statusChanged();
    void fullScreenChanged();
    void positionChanged();
    void rateChanged();
    void minimumRateChanged();
    void maximumRateChanged();
    void volumeChanged();
    void initialFetchFailed();
    void initialFetchFinished();

    void serviceNameChanged();

    void canPlayChanged();

public Q_SLOTS:
    void raise();
    void quit();

    void previous();
    void next();

    void play();
    void pause();
    void playPause();
    void stop();

    void seek(qlonglong offset);
    void setPosition(qlonglong position);
    void updatePosition();

    void openUrl(const QUrl &url);

private:
    QString m_serviceName;

    OrgFreedesktopDBusPropertiesInterface *m_propsInterface;
    OrgMprisMediaPlayer2Interface *m_interface;
    OrgMprisMediaPlayer2PlayerInterface *m_playerInterface;
    int m_fetchesPending;

    QString m_identity;
    QString m_iconName;

    Capabilities m_capabilities;

    QQmlPropertyMap *m_metadata;

    QString m_status;

    bool m_fullScreen;

    QDateTime m_lastPosUpdate;
    qlonglong m_position;
    qreal m_rate;
    qreal m_minimumRate;
    qreal m_maximumRate;
    qreal m_volume;

    void retrieveData();

    void setCapabilities(Capabilities cap);
    void setMetadata(const Metadata &map);

    void calculatePosition();

private Q_SLOTS:
    void propertiesFinished(QDBusPendingCallWatcher *watcher);
    void updateFromMap(const QVariantMap &map);
    void copyProperty(const QString &name, const QVariant &value,
                      const QVariant::Type &expectedType);
    void getPositionFinished(QDBusPendingCallWatcher *watcher);

};
Q_DECLARE_METATYPE(Mpris2Player::Capabilities)
Q_DECLARE_OPERATORS_FOR_FLAGS(Mpris2Player::Capabilities)
