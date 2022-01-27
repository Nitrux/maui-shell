#include "powerprofile.h"
#include <QDBusConnectionInterface>
#include <QDBusError>
#include <QDBusInterface>
#include <QDBusMetaType>
#include <QDBusPendingCallWatcher>
#include <QDBusReply>
#include <KService>

#include <QDebug>

static const char SOLID_POWERMANAGEMENT_SERVICE[] = "org.kde.Solid.PowerManagement";

PowerProfile::PowerProfile(QObject *parent) : QObject(parent)
{
    qDBusRegisterMetaType<QList<QVariantMap>>();
    qDBusRegisterMetaType<QList<QVariant>>();

    if (QDBusConnection::sessionBus().interface()->isServiceRegistered(SOLID_POWERMANAGEMENT_SERVICE))
    {

//        if (!QDBusConnection::sessionBus().connect(SOLID_POWERMANAGEMENT_SERVICE,
//                                                   QStringLiteral("/org/kde/Solid/PowerManagement/PolicyAgent"),
//                                                   QStringLiteral("org.kde.Solid.PowerManagement.PolicyAgent"),
//                                                   QStringLiteral("InhibitionsChanged"),
//                                                   this,
//                                                   SLOT(inhibitionsChanged(QList<InhibitionInfo>, QStringList)))) {
//            qDebug() << "error connecting to inhibition changes via dbus";
//        }



        if (!QDBusConnection::sessionBus().connect(SOLID_POWERMANAGEMENT_SERVICE,
                                                   QStringLiteral("/org/kde/Solid/PowerManagement/Actions/PowerProfile"),
                                                   QStringLiteral("org.kde.Solid.PowerManagement.Actions.PowerProfile"),
                                                   QStringLiteral("currentProfileChanged"),
                                                   this,
                                                   SLOT(setCurrentProfile(QString)))) {
            qDebug() << "error connecting to current profile changes via dbus";
        }

        if (!QDBusConnection::sessionBus().connect(SOLID_POWERMANAGEMENT_SERVICE,
                                                   QStringLiteral("/org/kde/Solid/PowerManagement/Actions/PowerProfile"),
                                                   QStringLiteral("org.kde.Solid.PowerManagement.Actions.PowerProfile"),
                                                   QStringLiteral("profileChoicesChanged"),
                                                   this,
                                                   SLOT(setProfileChoices(QStringList)))) {
            qDebug() << "error connecting to profile choices changes via dbus";
        }

        if (!QDBusConnection::sessionBus().connect(SOLID_POWERMANAGEMENT_SERVICE,
                                                   QStringLiteral("/org/kde/Solid/PowerManagement/Actions/PowerProfile"),
                                                   QStringLiteral("org.kde.Solid.PowerManagement.Actions.PowerProfile"),
                                                   QStringLiteral("performanceInhibitedReasonChanged"),
                                                   this,
                                                   SLOT(setPerformanceInhibitedReason(QString)))) {
            qDebug() << "error connecting to inhibition reason changes via dbus";
        }

        if (!QDBusConnection::sessionBus().connect(SOLID_POWERMANAGEMENT_SERVICE,
                                                   QStringLiteral("/org/kde/Solid/PowerManagement/Actions/PowerProfile"),
                                                   QStringLiteral("org.kde.Solid.PowerManagement.Actions.PowerProfile"),
                                                   QStringLiteral("performanceDegradedReasonChanged"),
                                                   this,
                                                   SLOT(setPerformanceDegradedReason(QString)))) {
            qDebug() << "error connecting to degradation reason changes via dbus";
        }

        if (!QDBusConnection::sessionBus().connect(SOLID_POWERMANAGEMENT_SERVICE,
                                                   QStringLiteral("/org/kde/Solid/PowerManagement/Actions/PowerProfile"),
                                                   QStringLiteral("org.kde.Solid.PowerManagement.Actions.PowerProfile"),
                                                   QStringLiteral("profileHoldsChanged"),
                                                   this,
                                                   SLOT(setProfileHolds(QList<QVariantMap>)))) {
            qDebug() << "error connecting to profile hold changes via dbus";
        }
    }

    auto profileMsg = QDBusMessage::createMethodCall(SOLID_POWERMANAGEMENT_SERVICE,
                                                     QStringLiteral("/org/kde/Solid/PowerManagement/Actions/PowerProfile"),
                                                     QStringLiteral("org.kde.Solid.PowerManagement.Actions.PowerProfile"),
                                                     QStringLiteral("currentProfile"));
    auto profileWatcher = new QDBusPendingCallWatcher(QDBusConnection::sessionBus().asyncCall(profileMsg));
    connect(profileWatcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher *watcher) {
        watcher->deleteLater();
        QDBusPendingReply<QString> reply = *watcher;
        if (reply.isError()) {
            return;
        }
        setCurrentProfile(reply.value());
    });

    auto choicesMsg = QDBusMessage::createMethodCall(SOLID_POWERMANAGEMENT_SERVICE,
                                                     QStringLiteral("/org/kde/Solid/PowerManagement/Actions/PowerProfile"),
                                                     QStringLiteral("org.kde.Solid.PowerManagement.Actions.PowerProfile"),
                                                     QStringLiteral("profileChoices"));
    auto choicesWatcher = new QDBusPendingCallWatcher(QDBusConnection::sessionBus().asyncCall(choicesMsg));
    connect(choicesWatcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher *watcher) {
        watcher->deleteLater();
        QDBusPendingReply<QStringList> reply = *watcher;
        if (reply.isError()) {
            return;
        }
        setProfileChoices(reply.value());
    });

    auto inhibitedMsg = QDBusMessage::createMethodCall(SOLID_POWERMANAGEMENT_SERVICE,
                                                       QStringLiteral("/org/kde/Solid/PowerManagement/Actions/PowerProfile"),
                                                       QStringLiteral("org.kde.Solid.PowerManagement.Actions.PowerProfile"),
                                                       QStringLiteral("performanceInhibitedReason"));
    auto inhibitedWatcher = new QDBusPendingCallWatcher(QDBusConnection::sessionBus().asyncCall(inhibitedMsg));
    connect(inhibitedWatcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher *watcher) {
        watcher->deleteLater();
        QDBusPendingReply<QString> reply = *watcher;
        if (reply.isError()) {
            return;
        }
        setPerformanceInhibitedReason(reply.value());
    });

    auto degradedMsg = QDBusMessage::createMethodCall(SOLID_POWERMANAGEMENT_SERVICE,
                                                      QStringLiteral("/org/kde/Solid/PowerManagement/Actions/PowerProfile"),
                                                      QStringLiteral("org.kde.Solid.PowerManagement.Actions.PowerProfile"),
                                                      QStringLiteral("performanceDegradedReason"));
    auto degradedWatcher = new QDBusPendingCallWatcher(QDBusConnection::sessionBus().asyncCall(degradedMsg));
    connect(degradedWatcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher *watcher) {
        watcher->deleteLater();
        QDBusPendingReply<QString> reply = *watcher;
        if (reply.isError()) {
            return;
        };
        setPerformanceDegradedReason(reply.value());
    });

    auto holdsMsg = QDBusMessage::createMethodCall(SOLID_POWERMANAGEMENT_SERVICE,
                                                   QStringLiteral("/org/kde/Solid/PowerManagement/Actions/PowerProfile"),
                                                   QStringLiteral("org.kde.Solid.PowerManagement.Actions.PowerProfile"),
                                                   QStringLiteral("profileHolds"));
    auto holdsWatcher = new QDBusPendingCallWatcher(QDBusConnection::sessionBus().asyncCall(holdsMsg));
    connect(holdsWatcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher *watcher) {
        watcher->deleteLater();
        QDBusPendingReply<QList<QVariantMap>> reply = *watcher;
        if (reply.isError()) {
            return;
        };
        setProfileHolds(reply.value());
    });
}

QString PowerProfile::currentProfile() const
{
    return m_currentProfile;
}

QStringList PowerProfile::profileChoices() const
{
    return m_profileChoices;
}

QString PowerProfile::performanceInhibitedReason() const
{
    return m_performanceInhibitedReason;
}

QString PowerProfile::performanceDegradedReason() const
{
    return m_performanceDegradedReason;
}

QList<QVariantMap> PowerProfile::profileHolds() const
{
    return m_profileHolds;
}

void PowerProfile::setCurrentProfile(QString currentProfile)
{
    if (m_currentProfile == currentProfile)
        return;

    m_currentProfile = currentProfile;
    emit currentProfileChanged(m_currentProfile);
}

void PowerProfile::setProfileChoices(QStringList profileChoices)
{
    if (m_profileChoices == profileChoices)
        return;

    m_profileChoices = profileChoices;
    emit profileChoicesChanged(m_profileChoices);
}

void PowerProfile::setPerformanceInhibitedReason(QString performanceInhibitedReason)
{
    if (m_performanceInhibitedReason == performanceInhibitedReason)
        return;

    m_performanceInhibitedReason = performanceInhibitedReason;
    emit performanceInhibitedReasonChanged(m_performanceInhibitedReason);
}

void PowerProfile::setPerformanceDegradedReason(QString performanceDegradedReason)
{
    if (m_performanceDegradedReason == performanceDegradedReason)
        return;

    m_performanceDegradedReason = performanceDegradedReason;
    emit performanceDegradedReasonChanged(m_performanceDegradedReason);
}

void PowerProfile::setProfileHolds(QList<QVariantMap> profileHolds)
{
qDebug() << "PROFILE HOLDS CHANGED" << profileHolds;
    QList<QVariantMap> out;
    std::transform(profileHolds.cbegin(), profileHolds.cend(), std::back_inserter(out), [this](const QVariantMap &hold) {
        QString prettyName;
        QString icon;
        populateApplicationData(hold[QStringLiteral("ApplicationId")].toString(), &prettyName, &icon);
        return QVariantMap{
            {QStringLiteral("Name"), prettyName},
            {QStringLiteral("Icon"), icon},
            {QStringLiteral("Reason"), hold[QStringLiteral("Reason")]},
            {QStringLiteral("Profile"), hold[QStringLiteral("Profile")]},
        };
    });

    m_profileHolds = out;
    emit profileHoldsChanged(m_profileHolds);
}

void PowerProfile::changeProfile(const QString &value)
{
    QDBusMessage msg = QDBusMessage::createMethodCall(QStringLiteral("org.kde.Solid.PowerManagement"),
                                                      QStringLiteral("/org/kde/Solid/PowerManagement/Actions/PowerProfile"),
                                                      QStringLiteral("org.kde.Solid.PowerManagement.Actions.PowerProfile"),
                                                      QStringLiteral("setProfile"));
    msg << value;

    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(QDBusConnection::sessionBus().asyncCall(msg), this);
    QObject::connect(watcher, &QDBusPendingCallWatcher::finished, this, [](QDBusPendingCallWatcher *watcher) {
        watcher->deleteLater();
    });
}

void PowerProfile::populateApplicationData(const QString &name, QString *prettyName, QString *icon)
{
    if (m_applicationInfo.contains(name))
    {
        const auto &info = m_applicationInfo.value(name);
        *prettyName = info.first;
        *icon = info.second;
    } else
    {
        KService::Ptr service = KService::serviceByStorageId(name + ".desktop");
        if (service) {
            *prettyName = service->property(QStringLiteral("Name"), QVariant::Invalid).toString(); // cannot be null
            *icon = service->icon();

            m_applicationInfo.insert(name, qMakePair(*prettyName, *icon));
        } else {
            *prettyName = name;
            *icon = name.section(QLatin1Char('/'), -1).toLower();
        }
    }
}
