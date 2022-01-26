#pragma once

#include <QObject>
#include <QList>
#include <QVariantMap>

class PowerProfile : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentProfile READ currentProfile WRITE setCurrentProfile NOTIFY currentProfileChanged)
    Q_PROPERTY(QStringList profileChoices READ profileChoices WRITE setProfileChoices NOTIFY profileChoicesChanged)
    Q_PROPERTY(QString performanceInhibitedReason READ performanceInhibitedReason WRITE setPerformanceInhibitedReason NOTIFY performanceInhibitedReasonChanged)
    Q_PROPERTY(QString performanceDegradedReason READ performanceDegradedReason WRITE setPerformanceDegradedReason NOTIFY performanceDegradedReasonChanged)
    Q_PROPERTY(QList<QVariantMap> profileHolds READ profileHolds WRITE setProfileHolds NOTIFY profileHoldsChanged)

public:
    explicit PowerProfile(QObject *parent = nullptr);

    QString currentProfile() const;

    QStringList profileChoices() const;

    QString performanceInhibitedReason() const;

    QString performanceDegradedReason() const;

    QList<QVariantMap> profileHolds() const;

public slots:

    void setCurrentProfile(QString currentProfile);

    void setProfileChoices(QStringList profileChoices);

    void setPerformanceInhibitedReason(QString performanceInhibitedReason);

    void setPerformanceDegradedReason(QString performanceDegradedReason);

    void setProfileHolds(QList<QVariantMap> profileHolds);

private:
    QString m_currentProfile;

    QStringList m_profileChoices;

    QString m_performanceInhibitedReason;

    QString m_performanceDegradedReason;

    QList<QVariantMap> m_profileHolds;

    QHash<QString, QPair<QString, QString>> m_applicationInfo; // <appname, <pretty name, icon>>

    void populateApplicationData(const QString &name, QString *prettyName, QString *icon);

signals:

    void currentProfileChanged(QString currentProfile);
    void profileChoicesChanged(QStringList profileChoices);
    void performanceInhibitedReasonChanged(QString performanceInhibitionReason);
    void performanceDegradedReasonChanged(QString performanceDegradedReason);
    void profileHoldsChanged(QList<QVariantMap> profileHolds);
};

