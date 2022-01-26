#pragma once
#include <QObject>
#include <QAbstractListModel>

#include <solid/battery.h>

#include <QDBusConnection>

class Battery : public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY(QString udi READ udi NOTIFY udiChanged FINAL)
    Q_PROPERTY(int capacity READ capacity NOTIFY capacityChanged FINAL)
    Q_PROPERTY(double energy READ energy NOTIFY energyChanged FINAL)
    Q_PROPERTY(bool isPowerSupply READ isPowerSupply NOTIFY isPowerSupplyChanged FINAL)
    Q_PROPERTY(int percent READ percent NOTIFY percentChanged FINAL)
    Q_PROPERTY(bool pluggedIn READ pluggedIn NOTIFY pluggedInChanged FINAL)
    Q_PROPERTY(QString prettyName READ prettyName NOTIFY prettyNameChanged FINAL)
    Q_PROPERTY(QString product READ product NOTIFY productChanged FINAL)
    Q_PROPERTY(QString type READ type NOTIFY typeChanged FINAL)
    Q_PROPERTY(QString vendor READ vendor NOTIFY vendorChanged FINAL)
    Q_PROPERTY(Solid::Battery::ChargeState state READ state NOTIFY stateChanged FINAL)
    Q_PROPERTY(QString stateName READ stateName NOTIFY stateChanged FINAL)
    Q_PROPERTY(qulonglong remainingTime READ remainingTime WRITE setRemainingTime NOTIFY remainingTimeChanged)

    Q_PROPERTY(qulonglong timeToEmpty READ timeToEmpty NOTIFY timeToEmptyChanged FINAL)
    Q_PROPERTY(qulonglong timeToFull READ timeToFull NOTIFY timeToFullChanged FINAL)

    Q_PROPERTY(int chargeStopThreshold READ chargeStopThreshold WRITE setChargeStopThreshold NOTIFY chargeStopThresholdChanged)

    explicit Battery(const Solid::Device &deviceBattery);
~Battery();
public:
    const QString &udi() const;

    const Solid::Battery::ChargeState &state() const;

    const QString &vendor() const;

    const QString &type() const;

    const QString &product() const;

    const QString &prettyName() const;

    bool pluggedIn() const;

    int percent() const;

    bool isPowerSupply() const;

    double energy() const;

    int capacity() const;

    qulonglong remainingTime() const;
    void setRemainingTime(qulonglong newRemainingTime);

    int chargeStopThreshold() const;
    void setChargeStopThreshold(int newChargeStopThreshold);

    const QString &stateName() const;

    qulonglong timeToEmpty() const;

    qulonglong timeToFull() const;

public slots:
    void setTimeToEmpty(qulonglong timeToEmpty);

    void setTimeToFull(qulonglong timeToFull);

signals:
    void udiChanged();

    void capacityChanged();

    void energyChanged();

    void isPowerSupplyChanged();

    void percentChanged();

    void pluggedInChanged();

    void prettyNameChanged();

    void productChanged();

    void typeChanged();

    void vendorChanged();

    void stateChanged();

    void remainingTimeChanged();

    void chargeStopThresholdChanged();

    void timeToEmptyChanged(qulonglong timeToEmpty);

    void timeToFullChanged(qulonglong timeToFull);

private:
    const Solid::Battery *m_battery;
    QString m_udi;
    QString m_vendor;
    QString m_product;
    QString m_type;
    Solid::Battery::ChargeState m_state;
    QString m_stateName;
   mutable QString m_prettyName;
    qulonglong m_remainingTime;
    int m_chargeStopThreshold;
    QString batteryType(const Solid::Battery *battery) const;
    QString batteryStateToString(int newState) const;

    void setState(const Solid::Battery::ChargeState &state);
    qulonglong m_timeToEmpty;
    qulonglong m_timeToFull;
};

class BatteryModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    explicit BatteryModel(QObject *parent = nullptr);
~BatteryModel();
    int rowCount(const QModelIndex &parent) const override final;
    QVariant data(const QModelIndex &index, int role) const override final;
    QHash<int, QByteArray> roleNames() const override final;

    void addBattery(Battery * battery);
    void removeBattery(Battery *battery);
    void removeBattery(const QString &udi);
    bool contains(const QString &udi) const;
    int indexOf(const QString &udi) const;
Battery * battery(const QString &udi);
    int count() const;

signals:
    void countChanged();

private:
    QVector<Battery*>m_batteries;
};

class BatteryInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(BatteryModel* batteries READ batteries NOTIFY batteriesChanged FINAL)
    Q_PROPERTY(bool hasBatteries READ hasBatteries NOTIFY hasBatteriesChanged FINAL)
    Q_PROPERTY(Battery* primaryBattery READ primaryBattery NOTIFY primaryBatteryChanged FINAL)
    Q_PROPERTY(bool acPluggedIn READ acPluggedIn NOTIFY acPluggedInChanged FINAL)

public:
    explicit BatteryInfo(QObject *parent = nullptr);

    BatteryModel *batteries() const;

    bool hasBatteries() const;

    Battery *primaryBattery() const;

    bool acPluggedIn() const;

private:
    BatteryModel *m_batteries;
    Battery *m_primaryBattery;
    bool m_hasBatteries = false;
    bool m_acPluggedIn = true;

    void deviceRemoved(const QString &udi);
    void deviceAdded(const QString &udi);

private slots:
    void onChargeStopThresholdChanged(const int &value);
    void onAcPlugStateChanged(const bool &value);

protected:
    void init();
    void setPrimaryBattery(Battery * battery);
    void setHasBatteries( const bool &value);

signals:
    void batteriesChanged();
    void hasBatteriesChanged();
    void primaryBatteryChanged();
    void acPluggedInChanged(bool acPluggedIn);
};

